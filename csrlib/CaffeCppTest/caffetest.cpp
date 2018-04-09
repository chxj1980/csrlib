#include "header.h"
#include <caffe/caffe.hpp>
#include <caffe\blob.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace caffe;
using namespace cv;
using namespace std;

//ʹ��opencv��������ʶ��
/** Global variables */
String face_cascade_name = "D:\\Opencv\\compile\\install\\etc\\haarcascades\\haarcascade_frontalface_alt2.xml";
String eyes_cascade_name = "D:\\Opencv\\compile\\install\\etc\\haarcascades\\haarcascade_eye.xml";
CascadeClassifier face_cascade;   //��������������  
CascadeClassifier eyes_cascade;   //�������۷�����  
String window_name = "Capture - Face detection";

/*����ָ�����ݣ�ǰ�򴫲�����*/
//! Note: data_ptrָ���Ѿ�����ã�ȥ��ֵ�ģ�������������ͼ��ĳ����Batch Size��������
void caffe_forward(boost::shared_ptr< Net<float> > & net, float *data_ptr)
{
	Blob<float>* input_blobs = net->input_blobs()[0];
	switch (Caffe::mode())
	{
	case Caffe::CPU:
		memcpy(input_blobs->mutable_cpu_data(), data_ptr,
			sizeof(float) * input_blobs->count());
		break;
	case Caffe::GPU:
		cudaMemcpy(input_blobs->mutable_gpu_data(), data_ptr,
			sizeof(float) * input_blobs->count(), cudaMemcpyHostToDevice);
		break;
	default:
		LOG(FATAL) << "Unknown Caffe mode.";
	}
	net->ForwardPrefilled();
}

/*����Feature������ֻ�ȡ���������е�Index*/
//! Note: Net��Blob��ָ��ÿ�����������ݣ���Feature Maps
// char *query_blob_name = "conv1";
unsigned int get_blob_index(boost::shared_ptr< Net<float> > & net, char *query_blob_name)
{
	std::string str_query(query_blob_name);
	vector< string > const & blob_names = net->blob_names();
	for (unsigned int i = 0; i != blob_names.size(); ++i)
	{
		if (str_query == blob_names[i])
		{
			return i;
		}
	}
	LOG(FATAL) << "Unknown blob name: " << str_query;
}

/*����Layer�����ֻ�ȡ���������е�Index*/
//! Note: Layer�������������в㣬���磬CaffeNet����23��
// char *query_layer_name = "conv1";
unsigned int get_layer_index(boost::shared_ptr< Net<float> > & net, char *query_layer_name)
{
	std::string str_query(query_layer_name);
	vector< string > const & layer_names = net->layer_names();
	for (unsigned int i = 0; i != layer_names.size(); ++i)
	{
		if (str_query == layer_names[i])
		{
			return i;
		}
	}
	LOG(FATAL) << "Unknown layer name: " << str_query;
}

void DetectFace(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces  
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 3);
	cout << faces.size() << endl;

	for (size_t i = 0; i < faces.size(); i++)
	{
		rectangle(frame, faces[i], Scalar(255, 0, 0), 2, 8, 0);

		Mat faceROI = frame_gray(faces[i]);
		std::vector<Rect> eyes;
#if 1
		//-- In each face, detect eyes  
		cout << "������ۿ�ʼ" << endl;
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
		cout << eyes.size() << endl;
		for (size_t j = 0; j < eyes.size(); j++)
		{
			Rect rect(faces[i].x + eyes[j].x, faces[i].y + eyes[j].y, eyes[j].width, eyes[j].height);
			rectangle(frame, rect, Scalar(0, 255, 0), 2, 8, 0);
		}
#endif
	}
	//-- Show what you got  
	//namedWindow(window_name, 2);
	imshow(window_name, frame);
}


int main(int argc, char** argv)
{
#if 0
	Blob<float> bf;
	cout << "size: " << bf.shape_string() << endl;
	bf.Reshape(1, 2, 3, 4);
	cout << "size: " << bf.shape_string() << endl;
#endif

#if 0
	Mat src = imread("D:\\Databases\\picData\\girls.jpeg");
#elif 0
	Mat src = imread("D:\\Databases\\myData\\lulu_1.jpg");
#endif

#if 0
	VideoCapture cap(0);
	if (!face_cascade.load(face_cascade_name))
		return -1;
	if (!eyes_cascade.load(eyes_cascade_name))
		return -1;
	Mat frame;
	while (1)
	{
		cap >> frame;
		DetectFace(frame);
		waitKey(10);
	}
#endif

#if 1
	/*��ʼ������*/
	char *proto = "E:\\Library\\caffe\\wincaffe\\ninjacaffe\\caffe\\models\\bvlc_reference_caffenet\\deploy.prototxt";
	Phase phase = TEST;
	Caffe::set_mode(Caffe::CPU);
	//Caffe::set_mode(Caffe::GPU);
	//Caffe.SetDevice(0);

	/*����caffenet����*/
	boost::shared_ptr<caffe::Net<float>> net(new caffe::Net<float>(proto, phase));
	//boost::shared_ptr<int> net(new int);

	/*����ѵ���õ�ģ��*/
	char *model = "E:\\Library\\caffe\\wincaffe\\ninjacaffe\\caffe\\models\\bvlc_reference_caffenet\\bvlc_reference_caffenet.caffemodel";
	net->CopyTrainedLayersFrom(model);

	/*����ģ����ÿ��Ľṹ���ò���*/
	NetParameter params;
	ReadNetParamsFromBinaryFileOrDie(model, &params);

	int numlayer = params.layer_size();
	for (int i = 0; i < numlayer; ++i)
	{
		//�ṹ���ò���:name,type,kernel,size, pad,stride��
		LOG(ERROR) << "Layer " << i << ":" << params.layer(i).name() << "\t" << params.layer(i).type();
		if (params.layer(i).type() == "Convolution")
		{
			ConvolutionParameter convparam = params.layer(i).convolution_param();
			char kernelSize[100];
			char pad[100];
			char stride[100];
			sprintf(kernelSize, "%d", convparam.kernel_size());
			sprintf(pad, "%d", convparam.pad());
			sprintf(stride, "%d", convparam.stride());
			LOG(ERROR) << "\t\tkernel size����" << kernelSize << ",pad����" << pad << ",stride:����" << stride;
		}
	}

	/*��ȡ���ؾ�ֵ*/
	char *mean_file = "H:\\Models\\Caffe\\imagenet_mean.binaryproto";
	Blob<float> image_mean;
	BlobProto blob_proto;
	const float *mean_ptr;
	unsigned int num_pixel;

	bool succeed = ReadProtoFromBinaryFile(mean_file, &blob_proto);
	if (succeed)
	{
		image_mean.FromProto(blob_proto);
		num_pixel = image_mean.count(); /* NCHW=1x3x256x256=196608 */
		mean_ptr = (const float *)image_mean.cpu_data();
	}

	/*����ָ�����ݣ�ǰ�򴫲�����*/

	/*����Feature������ֻ�ȡ���������е�Index*/

	/*��ȡ����ָ��Feature������*/
	//! Note: ����CaffeNet��deploy.prototxt�ļ�����Net����15��Blob����dataһֱ��prob    
	char *query_blob_name = "conv1"; /* data, conv1, pool1, norm1, fc6, prob, etc */
	unsigned int blob_id = get_blob_index(net, query_blob_name);

	boost::shared_ptr<Blob<float> > blob = net->blobs()[blob_id];
	unsigned int num_data = blob->count(); /* NCHW=10x96x55x55 */
	const float *blob_ptr = (const float *)blob->cpu_data();

	/*
	�����ļ��б���ȡ����������Ϊ�������ļ�
	���get_features.cpp�ļ���
    ��Ҫ������������
    �����ļ��б���ʽ��ѵ���õ����ƣ�ÿ��һ��ͼ�� �����ļ�ȫ·�����ո񡢱�ǩ��û�еĻ���������0��
    ����train_val����deploy��prototxt����д����feat.prototxt ��Ҫ�ǽ�������Ϊimage_data�㣬������prob��argmax��Ϊ��������ʺ�Top1/5Ԥ���ǩ��
    ����ָ�����������г������������ɸ��������ļ���������MATLAB��ȡ���ݣ����з���
	*/

	/*����Layer�����ֻ�ȡ���������е�Index*/

	/*��ȡָ��Layer��Ȩ������*/
	//! Note: ��ͬ��Net��Blob��Feature Maps��Layer��Blob��ָConv��FC�Ȳ��Weight��Bias
	char *query_layer_name = "conv1";
	const float *weight_ptr, *bias_ptr;
	unsigned int layer_id = get_layer_index(net, query_layer_name);
	boost::shared_ptr<Layer<float> > layer = net->layers()[layer_id];
	std::vector<boost::shared_ptr<Blob<float>  >> blobs = layer->blobs();
	if (blobs.size() > 0)
	{
		weight_ptr = (const float *)blobs[0]->cpu_data();
		bias_ptr = (const float *)blobs[1]->cpu_data();
	}
	//! Note: ѵ��ģʽ�£���ȡָ��Layer���ݶ����ݣ�������ƣ�Ψһ�������ǽ�cpu_data��Ϊcpu_diff


	/*�޸�ĳ���Weight����*/
	const float* data_ptr;          /* ָ���д�����ݵ�ָ�룬 Դ����ָ��*/
	//float* weight_ptr = NULL;       /* ָ��������ĳ��Ȩ�ص�ָ�룬Ŀ������ָ��*/
	unsigned int data_size = 0;         /* ��д��������� */
	char *layer_name = "conv1";     /* ��Ҫ�޸ĵ�Layer���� */

	//unsigned int layer_id = get_layer_index(net, query_layer_name);
	//boost::shared_ptr<Blob<float>> blob = net->layers()[layer_id]->blobs()[0];

	CHECK(data_size == blob->count());
	switch (Caffe::mode())
	{
	case Caffe::CPU:
		weight_ptr = blob->mutable_cpu_data();
		break;
	case Caffe::GPU:
		weight_ptr = blob->mutable_gpu_data();
		break;
	default:
		LOG(FATAL) << "Unknown Caffe mode";
	}
	//caffe_copy(blob->count(), (void*)data_ptr, (void*)weight_ptr);

	//! Note: ѵ��ģʽ�£��ֶ��޸�ָ��Layer���ݶ����ݣ��������
	// mutable_cpu_data��Ϊmutable_cpu_diff��mutable_gpu_data��Ϊmutable_gpu_diff


	/*�����µ�ģ��*/
	char* weights_file = "bvlc_reference_caffenet_new.caffemodel";
	NetParameter net_param;
	net->ToProto(&net_param, false);
	WriteProtoToBinaryFile(net_param, weights_file);

	printf("hello caffe\n");
	system("pause");
#endif
	return 0;
}