#ifndef _CSR_FACE_H_
#define _CSR_FACE_H_


#include <iostream>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

cv::String face_cascade_filename = "C:\\Opencv3.2.0\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml";
cv::String eyes_cascade_filename = "C:\\Opencv3.2.0\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";

//����������
cv::CascadeClassifier face_cascade;
//���۷�����
cv::CascadeClassifier eyes_cascade;

cv::String winName = "recogFace";


//�������
void detectedFace(cv::Mat frame);

//�����ɼ���
void ColectFace(cv::String strPath);




#endif
