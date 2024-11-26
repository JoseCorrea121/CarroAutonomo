import cv2
import numpy as np
import requests


# azulBajo = np.array([100,100,20], np.uint8)
# azulAlto = np.array([125,255,255], np.uint8)

# amarilloBajo = np.array([15,100,20], np.uint8)
# amarilloAlto = np.array([45,255,255], np.uint8)

verdeBajo = np.array([40,100,100], np.uint8)
verdeAlto = np.array([70,255,255], np.uint8)

redBajo1 = np.array([0, 100, 20], np.uint8)
redAlto1 = np.array([8, 255, 255], np.uint8)

redBajo2=np.array([175, 100, 20], np.uint8)
redAlto2=np.array([179, 255, 255], np.uint8)

while True:
    cap = cv2.VideoCapture('http://192.168.137.13/')
    ret, frame = cap.read()

    if ret == True:
        frameHSV = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        maskRed1 = cv2.inRange(frameHSV, redBajo1, redAlto1)
        maskRed2 = cv2.inRange(frameHSV, redBajo2, redAlto2)
        maskRed = cv2.add(maskRed1, maskRed2)
        maskGreen = cv2.inRange(frameHSV, verdeBajo, verdeAlto)

        contornos,_ = cv2.findContours(maskRed, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        for c in contornos:
            area = cv2.contourArea(c)
            if area > 4000:
                cv2.drawContours(frame, [c], 0, (255,0,0), 3)
                response = requests.get('http://192.168.137.14/?0=uno')

        contornos,_ = cv2.findContours(maskGreen, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        for c in contornos:
            area = cv2.contourArea(c)
            if area > 4000:
                cv2.drawContours(frame, [c], 0, (255,0,0), 3)
                response = requests.get('http://192.168.137.14/?1=uno')

        cv2.imshow('frame', frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()

# cd Documents\UVM\Microprocesadores\IA
