from maix import image, display, app, time, camera ,uart
from maix.v1.image import RGB2XYZ
import cv2
import numpy as np
import math
from math import *
from struct import pack
import re


disp = display.Display()
cam = camera.Camera()
# cam = camera.Camera(160, 120)
#cam =camera.Camera(1280,720)
#cam = camera.hmirror(0)
#cam = camera.skip_frames(time = 2000)
#cam = camera.awb_mode(1)
#cam = camera.skip_frames(time = 2000)



thresholdsH = [(30, 83, 47, 127, -128, 127), # red
              (0, 100, -125, -18, -70, 73), # green
              (0, 100, -20, 126, -128, -40)] # blue
thresholdsL = [(30, 83, 47, 127, -128, 127), # red
                (19, 93, -128, -26, -128, 127), # green
                (0, 100, -20, 126, -128, -40)] # blue

thresholds = [(25, 83, 30, 127, -128, 127), # generic_red_thresholds
              (19, 93, -128, -26, -128, 127), # generic_green_thresholds
              (0, 100, -20, 126, -128, -40)] # generic_blue_thresholds

thresholds_circle = [(20, 80, 24, 127, -9, 42), # generic_red_thresholds
                    (0,81, -128, -9, 127, -128), # generic_green_thresholds
                    (0,100,-20,127,-128,-50)] # generic_blue_thresholds


def scan_color_circle(color_code):
    setcenter = [160, 120]
    min_area_threshold = 5000  # 最小面积阈值
    max_area_threshold = 100000  # 最大面积阈值

    colorsholds = {
        'green': {'lower': np.array([35, 43, 46]), 'upper': np.array([80, 255, 255])},
        'red1': {'lower': np.array([20, 100, 46]), 'upper': np.array([10, 255, 255])},
        'red2': {'lower': np.array([170, 100, 46]), 'upper': np.array([180, 255, 255])},
        'blue': {'lower': np.array([100, 30, 46]), 'upper': np.array([124, 255, 255])}
    }

    color = [[0, 0], [0, 0], [0, 0]]
    
    cam.set_resolution(width=320, height=240)
    img = cam.read()
    img = img.lens_corr(strength=1.5)	# 调整strength的值直到画面不再畸变
    
    img = image.image2cv(img, ensure_bgr=True, copy=False)
    hsv_img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    
    # 初始化结果
    results = {'green': [0, 0], 'red': [0, 0], 'blue': [0, 0]}

    # 根据color_code选择要识别的颜色
    if color_code == 0xC0:
        colors_to_detect = ['red1', 'red2']
    elif color_code == 0xC1:
        colors_to_detect = ['green']
    elif color_code == 0xC2:
        colors_to_detect = ['blue']
    else:
        colors_to_detect = []  # 如果没有匹配的color_code，则不检测任何颜色

    for color_name in colors_to_detect:
        if color_name == 'red1' or color_name == 'red2':
            if color_name == 'red1':
                mask1 = cv2.inRange(hsv_img, colorsholds[color_name]['lower'], colorsholds[color_name]['upper'])
            elif color_name == 'red2':
                mask2 = cv2.inRange(hsv_img, colorsholds[color_name]['lower'], colorsholds[color_name]['upper'])
            if 'mask1' in locals() and 'mask2' in locals():
                mask = cv2.bitwise_or(mask1, mask2)
                color_name = 'red'  # 统一处理为红色
            else:
                continue
        else:
            mask = cv2.inRange(hsv_img, colorsholds[color_name]['lower'], colorsholds[color_name]['upper'])
    
    # for color_name, color_range in colorsholds.items():
    #     if color_name == 'red1' or color_name == 'red2':
    #         if color_name == 'red1':
    #             mask1 = cv2.inRange(hsv_img, color_range['lower'], color_range['upper'])
    #         elif color_name == 'red2':
    #             mask2 = cv2.inRange(hsv_img, color_range['lower'], color_range['upper'])
    #         if 'mask1' in locals() and 'mask2' in locals():
    #             mask = cv2.bitwise_or(mask1, mask2)
    #             color_name = 'red'  # 统一处理为红色
    #         else:
    #             continue
    #     else:
    #         mask = cv2.inRange(hsv_img, color_range['lower'], color_range['upper'])
        
        # 形态学操作：膨胀
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5)) #np.ones((5, 5), np.uint8)
        gaussianblur_mask = cv2.GaussianBlur(mask,(5,5),1)
        #morphologyEx_mask = cv2.morphologyEx(gaussianblur_mask,cv2.MORPH_CLOSE,kernel)
        erode_mask =cv2.dilate(gaussianblur_mask,kernel,iterations=3) 

        # 初始化最大面积和对应的轮廓
        max_area = 0
        largest_circle = None
        
        # 找到轮廓
        contours, _ = cv2.findContours(erode_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        for contour in contours:
            area = cv2.contourArea(contour)
            if min_area_threshold <= area <= max_area_threshold and area > max_area:
                max_area = area
                largest_circle = contour
                
        if largest_circle is not None:
            M = cv2.moments(largest_circle)
            if M["m00"] != 0:
                center_x = int(M["m10"] / M["m00"])
                center_y = int(M["m01"] / M["m00"])
                
                # 在原图上绘制轮廓
                cv2.drawContours(img, [largest_circle], -1, (0, 255, 0), 2)
                cv2.circle(img, (center_x, center_y), 5, (0, 0, 255), -1)

                results[color_name] = [center_x - setcenter[0], center_y - setcenter[1]]
                if color_name =='red' :
                    color[0][0] = center_x -setcenter[0]
                    color[0][1] = center_y -setcenter[1]
                if color_name == 'green' :
                    color[1][0] = center_x -setcenter[0]
                    color[1][1] = center_y -setcenter[1]          
                if color_name == 'blue' :
                    color[2][0] = center_x -setcenter[0] 
                    color[2][1] = center_y -setcenter[1]           


    # 显示结果
    img = image.cv2image(img)
    disp.show(img)    
    print(color)
    return color


def scan_color_plus():
    setcenter = [160, 120]
    min_area_threshold = 1500    # 最小面积阈值
    max_area_threshold = 30000  # 最大面积阈值

    colorsholds = {
        'green': {'lower': np.array([35, 43, 46]), 'upper': np.array([80, 255, 255])},
        'red1': {'lower': np.array([0, 43, 46]), 'upper': np.array([10, 255, 255])},
        'red2': {'lower': np.array([156, 43, 46]), 'upper': np.array([180, 255, 255])},
        'blue': {'lower': np.array([100, 70, 50]), 'upper': np.array([130, 255, 255])}
    }

    color = [[0, 0], [0, 0], [0, 0]]
    
    cam.set_resolution(width=320, height=240)
    img = cam.read()
    
    img = image.image2cv(img, ensure_bgr=True, copy=False)
    hsv_img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    
    # 初始化结果
    results = {'green': [0, 0], 'red': [0, 0], 'blue': [0, 0]}
    
    for color_name, color_range in colorsholds.items():
        if color_name == 'red1' or color_name == 'red2':
            if color_name == 'red1':
                mask1 = cv2.inRange(hsv_img, color_range['lower'], color_range['upper'])
            elif color_name == 'red2':
                mask2 = cv2.inRange(hsv_img, color_range['lower'], color_range['upper'])
            if 'mask1' in locals() and 'mask2' in locals():
                mask = cv2.bitwise_or(mask1, mask2)
                color_name = 'red'  # 统一处理为红色
            else:
                continue
        else:
            mask = cv2.inRange(hsv_img, color_range['lower'], color_range['upper'])
        
        # 形态学操作：膨胀
        kernel = np.ones((5, 5), np.uint8)#cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))
        #gaussianblur_mask = cv2.GaussianBlur(mask,(5,5),0)
        morphologyEx_mask = cv2.morphologyEx(mask,cv2.MORPH_OPEN,kernel)

        # 初始化最大面积和对应的轮廓
        max_area = 0
        largest_circle = None
        
        # 找到轮廓
        contours, _ = cv2.findContours(morphologyEx_mask, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
        for contour in contours:
            #cv2.drawContours(img, [contour], -1, (0, 255, 0), 2)
            area = cv2.contourArea(contour)
            if min_area_threshold <= area <= max_area_threshold and area > max_area:
                max_area = area
                largest_circle = contour
                
        if largest_circle is not None:
            M = cv2.moments(largest_circle)
            if M["m00"] != 0:
                center_x = int(M["m10"] / M["m00"])
                center_y = int(M["m01"] / M["m00"])
                
                # 在原图上绘制轮廓
                cv2.drawContours(img, [largest_circle], -1, (0, 255, 0), 2)
                cv2.circle(img, (center_x, center_y), 5, (0, 0, 255), -1)

                results[color_name] = [center_x - setcenter[0], center_y - setcenter[1]]
                if color_name =='red' :
                    color[0][0] = center_x -setcenter[0]
                    color[0][1] = center_y -setcenter[1]
                if color_name == 'green' :
                    color[1][0] = center_x -setcenter[0]
                    color[1][1] = center_y -setcenter[1]          
                if color_name == 'blue' :
                    color[2][0] = center_x -setcenter[0]
                    color[2][1] = center_y -setcenter[1]           


    # 显示结果
    img = image.cv2image(img)
    disp.show(img)    
    print(color)
    return color

# def scan_color_plus():
#     setcenter =[320,240]
#     color = [[0,0],[0,0],[0,0]]
#     cam.set_resolution(width=640, height=480)
#     img =cam.read()
#     for r in img.find_blobs([thresholds[0]],pixels_threshold=200, area_threshold=200,merge=True):   
#             # thresholds是颜色的阈值，注意：这个参数是一个列表，可以包含多个颜色。如果你只需要一个颜色，
#             #那么在这个列表中只需要有一个颜色值，如果你想要多个颜色阈值，那这个列表就需要多个颜色阈值.
#             #注意：在返回的色块对象blob可以调用code方法，来判断是什么颜色的色块。

#             #pixels_threshold 像素个数阈值，如果色块像素数量小于这个值，会被过滤掉
#             #area_threshold 面积阈值，如果色块被框起来的面积小于这个值，会被过滤掉
#             #merge 合并，如果设置为True，那么合并所有重叠的blob为一个
#             #find_blobs返回多个blob的列表
#         img.draw_rect(r.x(),r.y(),r.w(),r.h(),image.Color.from_rgb(255, 0, 0),2)
#         img.draw_cross(r.cx(),r.cy(),image.Color.from_rgb(255, 255, 255),5,2)
#         color[0][0] = r.cx()-setcenter[0]
#         color[0][1] = r.cy()-setcenter[1]
#     for g in img.find_blobs([thresholds[1]], pixels_threshold=200, area_threshold=500,merge=True):
#             # These values depend on the blob not being circular - otherwise they will be shaky.

#             # These values are stable all the time.
#         img.draw_rect(g.x(),g.y(),g.w(),g.h(),image.Color.from_rgb(255, 0, 0),2)
#         img.draw_cross(g.cx(),g.cy(),image.Color.from_rgb(255, 255, 255),5,2)
#             # Note - the blob rotation is unique to 0-180 only.
#         color[1][0] = g.cx()-setcenter[0]
#         color[1][1] = g.cy()-setcenter[1]
#     for b in img.find_blobs([thresholds[2]], pixels_threshold=200, area_threshold=500,merge=True):
#             # These values depend on the blob not being circular - otherwise they will be shaky.

#             # These values are stable all the time.
#         img.draw_rect(b.x(),b.y(),b.w(),b.h(),image.Color.from_rgb(255, 0, 0),2)
#         img.draw_cross(b.cx(),b.cy(),image.Color.from_rgb(255, 255, 255),5,2)
#         color[2][0] = b.cx()-setcenter[0]
#         color[2][1] = b.cy()-setcenter[1]
#     disp.show(img)
#     print(color)
#     return color
   

def recv_handle(data : bytes):
    color_select =None
    tmp =None
    RecvCode =None
    # tmp1 =hex(data[0])
    # tmp2 =hex(data[1])
    # tmp3 =hex(data[2])
    # tmp4 =hex(data[3])

    # print(tmp1,tmp2,tmp3,tmp4)
    # if tmp1 == "0xaa" and tmp2 == "0xa5" and tmp4 =="0xbb":
    #     if tmp3=="0xb1" :
    #         RecvCode = 0XB1
    #         print("qrcodes")
    #         return RecvCode
    #     if tmp3 =="0xb2" :
    #         RecvCode =0XB2
    #         return RecvCode
    #     if tmp3 == "0xb3" :
    #         RecvCode =0XB3
    #         return RecvCode
    #print(hex(data[0]))
    # print(len(data))
    print(data)
    tmp3 = hex(data[0])
    if tmp3=="0xb1" :
        RecvCode = 0XB1
        print("qrcodes")
        return RecvCode,color_select
    if tmp3 =="0xb2" :
        RecvCode =0XB2   
        return RecvCode,color_select
    if tmp3 == "0xb3" :
        RecvCode =0XB3
        if len(data)>=2:
            tmp = hex(data[1])
        else :
            tmp =None
        if tmp == "0xc0" :
            color_select =0XC0
        if tmp == "0xc1" :
            color_select =0XC1
        if tmp == "0xc2" :
            color_select =0XC2
        if tmp == None :
            color_select =None
        return RecvCode,color_select
    else :
        return RecvCode,color_select

            


device = "/dev/serial0"
serial = uart.UART(device, 115200)
#ports = uart.list_devices() # 列出当前可用的串口
#print(ports)
#serial.write_str("hello\n")

#通信标准 接收 0XAA 0XA5(命令码) (0XB1 扫描二维码 0XB2 扫描物料 0XB3 扫描圆环) 0XBB
#发送 0XAA 0X5A(数据码) (0XB1 +识别到的内容) 0XBB
#    0XAA 0X5A(数据码) (0XB2 颜色(0XC0 red 0XC1 green 0XC2 blue) +对应颜色中心坐标) 0XBB
#    0XAA 0X5A(数据码) (0XB3 颜色(0XC0 red 0XC1 green 0XC2 blue) +对应颜色中心坐标) 0XBB

flag =[0,0,0]

color_select_flag =[0,0,0]
RecvCode =0
color_select =None

while not app.need_exit():
    data =serial.read(-1,2)
    if data != b'':
       RecvCode,color_select = recv_handle(data)
       if RecvCode ==0XB1 :
        flag=[1,0,0]
       if RecvCode ==0XB2 :
        flag=[0,1,0]
       if RecvCode ==0XB3 :
        flag=[0,0,1]
        if color_select ==0XC0 :
            color_select_flag=[1,0,0]
        if color_select ==0XC1 :
            color_select_flag=[0,1,0]
        if color_select ==0XC2 :
            color_select_flag=[0,0,1] 
        if color_select ==None :
            color_select_flag=[0,0,0]


              



    if flag[0] ==1:
        cam.set_resolution(width=640, height=480)
        img =cam.read()
        QRcodes = img.find_qrcodes()
        for qr in QRcodes:
            corners = qr.corners()
            for i in range(4):
                img.draw_line(corners[i][0], corners[i][1], corners[(i + 1) % 4][0], corners[(i + 1) % 4][1], image.COLOR_RED)
            img.draw_string(qr.x(), qr.y() - 15, qr.payload(), image.COLOR_RED)
            bytes_content = b'\xAA\x5A\xB1'
            # 使用正则表达式提取数字部分
            numbers = re.findall(r'\d+', qr.payload())   
            for number in numbers:
                number_tmp =int(number)
                bytes_content += pack("<H",number_tmp)  
            bytes_content += b'\xBB'
            serial.write(bytes_content)
            #print(qr.payload())
            #print(bytes_content)
        disp.show(img)
    if flag[1] ==1 :
        #print("scan color")
        cam.set_resolution(width=320, height=240)
        color = scan_color_plus()
        bytes_content = b'\xAA\x5A\xB2'
        #red
        bytes_content += b'\xC0'
        bytes_content += pack("<h",int(color[0][0]))
        bytes_content += pack("<h",int(color[0][1]))
        #green
        bytes_content += b'\xC1'
        bytes_content += pack("<h",int(color[1][0]))
        bytes_content += pack("<h",int(color[1][1]))
        #blue
        bytes_content += b'\xC2'
        bytes_content += pack("<h",int(color[2][0]))
        bytes_content += pack("<h",int(color[2][1]))
        bytes_content += b'\xBB'
        serial.write(bytes_content)
        #print(bytes_content)
    if flag[2] ==1 :
        if color_select_flag[0]==1:
                color = scan_color_circle(0XC0)
                bytes_content = b'\xAA\x5A\xB3'
                #red
                bytes_content += b'\xC0'
                bytes_content += pack("<h",int(color[0][0]))
                bytes_content += pack("<h",int(color[0][1]))
                #green
                bytes_content += b'\xC1'
                bytes_content += pack("<h",int(color[1][0]))
                bytes_content += pack("<h",int(color[1][1]))
                #blue
                bytes_content += b'\xC2'
                bytes_content += pack("<h",int(color[2][0]))
                bytes_content += pack("<h",int(color[2][1]))
                bytes_content += b'\xBB'
                serial.write(bytes_content)
                #print(bytes_content)
        if color_select_flag[1]==1:
                color = scan_color_circle(0XC1)
                bytes_content = b'\xAA\x5A\xB3'
                #red
                bytes_content += b'\xC0'
                bytes_content += pack("<h",int(color[0][0]))
                bytes_content += pack("<h",int(color[0][1]))
                #green
                bytes_content += b'\xC1'
                bytes_content += pack("<h",int(color[1][0]))
                bytes_content += pack("<h",int(color[1][1]))
                #blue
                bytes_content += b'\xC2'
                bytes_content += pack("<h",int(color[2][0]))
                bytes_content += pack("<h",int(color[2][1]))
                bytes_content += b'\xBB'
                serial.write(bytes_content)
                #print(bytes_content)
        if color_select_flag[2]==1:
                color = scan_color_circle(0XC2)
                bytes_content = b'\xAA\x5A\xB3'
                #red
                bytes_content += b'\xC0'
                bytes_content += pack("<h",int(color[0][0]))
                bytes_content += pack("<h",int(color[0][1]))
                #green
                bytes_content += b'\xC1'
                bytes_content += pack("<h",int(color[1][0]))
                bytes_content += pack("<h",int(color[1][1]))
                #blue
                bytes_content += b'\xC2'
                bytes_content += pack("<h",int(color[2][0]))
                bytes_content += pack("<h",int(color[2][1]))
                bytes_content += b'\xBB'
                serial.write(bytes_content)
                #print(bytes_content)
        if  color_select_flag[0] == 0 and color_select_flag[1] == 0 and color_select_flag[2] == 0 :
                color = [[0, 0], [0, 0], [0, 0]]
                bytes_content = b'\xAA\x5A\xB3'
                #red
                bytes_content += b'\xC3'
                bytes_content += pack("<h",int(color[0][0]))
                bytes_content += pack("<h",int(color[0][1]))
                #green
                bytes_content += b'\xC3'
                bytes_content += pack("<h",int(color[1][0]))
                bytes_content += pack("<h",int(color[1][1]))
                #blue
                bytes_content += b'\xC3'
                bytes_content += pack("<h",int(color[2][0]))
                bytes_content += pack("<h",int(color[2][1]))
                bytes_content += b'\xBB'
                serial.write(bytes_content)
                data =serial.read(-1,2)
                if data != b'':
                    _,color_select = recv_handle(data)
                    if color_select ==0XC0 :
                        color_select_flag=[1,0,0]
                    if color_select ==0XC1 :
                        color_select_flag=[0,1,0]
                    if color_select ==0XC2 :
                        color_select_flag=[0,0,1] 
                    if flag[2]==1 and color_select ==None :
                        color_select_flag=[0,0,0]

