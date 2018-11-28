#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <list>
#include <math.h>

using namespace std;
using namespace cv;

int reflect(int M, int x)
{
	if (x < 0)
	{
		return -x - 1;
	}
	if (x >= M)
	{
		return 2 * M - x - 1;
	}
	return x;
}

int circular(int M, int x)
{
	if (x < 0)
		return x + M;
	if (x >= M)
		return x - M;
	return x;
}


void noBorderProcessing(Mat src, Mat dst, float Kernel[][3])
{

	float sum;
	for (int y = 1; y < src.rows - 1; y++) {
		for (int x = 1; x < src.cols - 1; x++) {
			sum = 0.0;
			for (int k = -1; k <= 1; k++) {
				for (int j = -1; j <= 1; j++) {
					sum = sum + Kernel[j + 1][k + 1] * src.at<uchar>(y - j, x - k);
				}
			}
			dst.at<uchar>(y, x) = sum;
		}
	}
}

void refletedIndexing(Mat src, Mat dst, float Kernel[][3])
{
	float sum, x1, y1;
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			sum = 0.0;
			for (int k = -1; k <= 1; k++) {
				for (int j = -1; j <= 1; j++) {
					x1 = reflect(src.cols, x - j);
					y1 = reflect(src.rows, y - k);
					sum = sum + Kernel[j + 1][k + 1] * src.at<uchar>(y1, x1);
				}
			}
			dst.at<uchar>(y, x) = sum;
		}
	}
}

void circularIndexing(Mat src, Mat dst, float Kernel[][3])
{
	float sum, x1, y1;
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			sum = 0.0;
			for (int k = -1; k <= 1; k++) {
				for (int j = -1; j <= 1; j++) {
					x1 = circular(src.cols, x - j);
					y1 = circular(src.rows, y - k);
					sum = sum + Kernel[j + 1][k + 1] * src.at<uchar>(y1, x1);
				}
			}
			dst.at<uchar>(y, x) = sum;
		}
	}
}


Mat WallisFunction(Mat src, int Md, int Sd,float Amax, float r) {
	//float Gauss[3][3] = {
	//					{1 / 9.0, 1 / 9.0, 1 / 9.0},
	//					{1 / 9.0, 1 / 9.0, 1 / 9.0},
	//					{1 / 9.0,1 / 9.0, 1 / 9.0}
	//};
	Mat F = src.clone();
	Mat M = src.clone();
	Mat S;
	F.convertTo(S, CV_32FC1);
	Mat Fsquare;
	F.convertTo(Fsquare, CV_32FC1);
	Mat Msquare;
	F.convertTo(Msquare, CV_32FC1);
	Mat Wallis = src.clone();



	

	for (int i = 0; i < 5; i++) {
		GaussianBlur(M, M, Size(3, 3), 0, 0);
	}
	


	for (int i = 0; i < F.cols; i++) {
		for (int j = 0; j < F.rows; j++) {
			Fsquare.at<float>(Point(j, i)) = std::pow(F.at<uchar>(Point(j, i)), 2);
		}
	}


	for (int i = 0; i < 5; i++) {
		GaussianBlur(Fsquare, Fsquare, Size(3, 3), 0, 0);
	}
	


	for (int i = 0; i < F.cols; i++) {
		for (int j = 0; j < F.rows; j++) {
			Msquare.at<float>(Point(j, i)) = std::pow(M.at<uchar>(Point(j, i)), 2);
		}
	}
	


	for (int i = 0; i < F.cols; i++) {
		for (int j = 0; j < F.rows; j++) {
			
			S.at<float>(Point(j, i)) = Fsquare.at<float>(Point(j, i)) - Msquare.at<float>(Point(j, i));
			S.at<float>(Point(j, i)) = std::abs(S.at<float>(Point(j, i)));
		}
	}





	for (int i = 0; i < F.cols; i++) {
		for (int j = 0; j < F.rows; j++) {
			S.at<float>(Point(j, i)) =std::sqrt(S.at<float>(Point(j, i)));
		}
	}

	for (int i = 0; i < F.cols; i++) {
		for (int j = 0; j < F.rows; j++) {
			int test = ((float)(F.at<uchar>(Point(j, i)) - M.at<uchar>(Point(j, i)))) * ((float)(Amax*Sd)) / (float)(Sd + Amax * (float)S.at<float>(Point(j, i))) + (r*Md + (1 - r)*M.at<uchar>(Point(j, i)));
			if (test < 0) {
				Wallis.at<uchar>(Point(j, i)) = 0;
			}
			else if (test > 255) {
				Wallis.at<uchar>(Point(j, i)) = 255;
			}
			else {
				Wallis.at<uchar>(Point(j, i)) = test;
			}
		}
	}
	return Wallis;
}





int main(int argc, char** argv)
{

	Mat src, dst;

	/// Load image
	src = imread(argv[1], 0);

	if (!src.data)
	{
		return -1;
	}


	//int hist[256];
	//for (int i = 0; i < 256; i++) {
	//	hist[i] = 0;
	//}

	//for (int i = 0; i < src.cols; i++) {
	//	for (int j = 0; j < src.rows; j++) {
	//		hist[(int)src.at<uchar>(Point(j, i))]++;
	//	}
	//}
	//float max = 0;
	//for (int i = 0; i < 256; i++) {
	//	if (max < hist[i]) {
	//		max = hist[i];
	//	}
	//}


	//for (int i = 0; i < 256; i++) {
	//	float tmp = hist[i];
	//	hist[i] = (tmp / max) * 255;
	//}

	//Mat histImage0(255, 255, CV_8UC1, Scalar(0, 0, 0));
	//for (int i = 0; i < 256; i++) {
	//	line(histImage0, Point(i, 255), Point(i, 255 - hist[i]), Scalar(255, 0, 0), 1, 8, 0);
	//}

	//namedWindow("hist", WINDOW_AUTOSIZE);
	//imshow("hist", histImage0);


	//namedWindow("Source", WINDOW_AUTOSIZE);
	//imshow("Source", src);
	//waitKey(0);
	//destroyAllWindows();




	////////////////////////////// HIST 1 /////////////////////////////////////

	//int hist2[256];

	//for (int i = 0; i < 256; i++) {
	//	float tmp = hist[i];
	//	hist2[i] = 255 * sqrt(tmp / 255);
	//}





	//Mat histImage2(src.rows, src.cols, CV_8UC1, Scalar(0, 0, 0));


	//for (int i = 0; i < src.cols; i++) {
	//	for (int j = 0; j < src.rows; j++) {
	//		float tmp = src.at<uchar>(Point(j, i));
	//		histImage2.at<uchar>(Point(j, i)) = 255 * std::sqrt(tmp / 255.0);
	//	}
	//}
	////
	//namedWindow("Bright", WINDOW_AUTOSIZE);
	//imshow("Bright", histImage2);
	//


	//for (int i = 0; i < 256; i++) {
	//	hist[i] = 0;
	//}

	//for (int i = 0; i < histImage2.cols; i++) {
	//	for (int j = 0; j < histImage2.rows; j++) {
	//		hist[(int)histImage2.at<uchar>(Point(j, i))]++;
	//	}
	//}
	//max = 0;
	//for (int i = 0; i < 256; i++) {
	//	if (max < hist[i]) {
	//		max = hist[i];
	//	}
	//}


	//for (int i = 0; i < 256; i++) {
	//	float tmp = hist[i];
	//	hist[i] = (tmp / max) * 255;
	//}

	//Mat histtmp2(255, 255, CV_8UC1, Scalar(0, 0, 0));
	//for (int i = 0; i < 256; i++) {
	//	line(histtmp2, Point(i, 255), Point(i, 255 - hist[i]), Scalar(255, 0, 0), 2, 8, 0);
	//}

	//namedWindow("Bright Histogram", WINDOW_AUTOSIZE);
	//imshow("Bright Histogram", histtmp2);
	//waitKey(0);
	//destroyAllWindows();




	//////////////////////////// HIST 3 ///////////////////////////////////////

	//int hist3[256];

	//for (int i = 0; i < 256; i++) {
	//	float tmp = hist[i];
	//	hist3[i] = 255 * (tmp / 255)*(tmp / 255);
	//}





	//Mat histImage3(src.rows, src.cols, CV_8UC1, Scalar(0, 0, 0));

	//for (int i = 0; i < src.cols; i++) {
	//	for (int j = 0; j < src.rows; j++) {
	//		float tmp = src.at<uchar>(Point(j, i));
	//		histImage3.at<uchar>(Point(j, i)) = 255 * (tmp / 255.0)*(tmp / 255.0);
	//	}
	//}



	//namedWindow("Dark", WINDOW_AUTOSIZE);
	//imshow("Dark", histImage3);

	//for (int i = 0; i < 256; i++) {
	//	hist[i] = 0;
	//}

	//for (int i = 0; i < histImage3.cols; i++) {
	//	for (int j = 0; j < histImage3.rows; j++) {
	//		hist[(int)histImage3.at<uchar>(Point(j, i))]++;
	//	}
	//}
	//max = 0;
	//for (int i = 0; i < 256; i++) {
	//	if (max < hist[i]) {
	//		max = hist[i];
	//	}
	//}



	//for (int i = 0; i < 256; i++) {
	//	float tmp = hist[i];
	//	hist[i] = (tmp / max) * 255;
	//}

	//Mat histtmp3(255, 255, CV_8UC1, Scalar(0, 0, 0));
	//for (int i = 0; i < 256; i++) {
	//	line(histtmp3, Point(i, 255), Point(i, 255 - hist[i]), Scalar(255, 0, 0), 2, 8, 0);
	//}

	//namedWindow("Dark Histogram", WINDOW_AUTOSIZE);
	//imshow("Dark Histogram", histtmp3);
	//waitKey(0);
	//destroyAllWindows();


	//////////////////////////// EQUALIZE /////////////////////////

	//const float K = 20;
	//int N = src.cols * src.rows;
	//float NperK = N / K;

	////cout << "N = " << N << endl;
	////cout << "K = " << K << endl;

	////cout << "N//K = " << NperK << endl;



	//int histtemp[256];
	//for (int i = 0; i < 256; i++) {
	//	histtemp[i] = 0;
	//}

	//for (int i = 0; i < src.cols; i++) {
	//	for (int j = 0; j < src.rows; j++) {
	//		histtemp[(int)src.at<uchar>(Point(j, i))]++;
	//	}
	//}

	//int lut[256];
	//for (int i = 0; i < 256; i++) {
	//	lut[i] = 0;
	//}

	//int counter = 0;
	//int stepper = 0;
	//for (int i = 0; i < 256; i++) {
	//	if (counter < NperK) {
	//		counter += histtemp[i];
	//		lut[i] = stepper * 256.0 / K;
	//	}
	//	else {
	//		counter = 0;
	//		lut[i] = stepper * 256.0 / K;
	//		stepper++;
	//	}
	//}



	//Mat histImage4(src.rows, src.cols, CV_8UC1, Scalar(0, 0, 0));

	//for (int i = 0; i < src.cols; i++) {
	//	for (int j = 0; j < src.rows; j++) {
	//		int tmp = src.at<uchar>(Point(j, i));
	//		histImage4.at<uchar>(Point(j, i)) = lut[tmp];
	//	}
	//}

	//normalize(histImage4, histImage4, 0, 255, NORM_MINMAX, CV_8UC1);


	//float hist4[256];
	//for (int i = 0; i < 256; i++) {
	//	hist4[i] = 0;
	//}

	//for (int i = 0; i < histImage4.cols; i++) {
	//	for (int j = 0; j < histImage4.rows; j++) {
	//		hist4[(int)histImage4.at<uchar>(Point(j, i))]++;
	//	}
	//}





	//max = 0;
	//for (int i = 0; i < 256; i++) {
	//	if (max < hist4[i]) {
	//		max = hist4[i];
	//	}
	//}

	//for (int i = 0; i < 256; i++) {
	//	float tmp = hist4[i];
	//	hist4[i] = (tmp / max) * 255;
	//}




	//Mat histtmp4(255, 255, CV_8UC1, Scalar(0, 0, 0));
	//for (int i = 0; i < 256; i++) {
	//	line(histtmp4, Point(i, 255), Point(i, 255 - hist4[i]), Scalar(255, 0, 0), 1, 8, 0);
	//}

	//namedWindow("Equalized", WINDOW_AUTOSIZE);
	//imshow("Equalized", histtmp4);


	//namedWindow("Equalized Histogram", WINDOW_AUTOSIZE);
	//imshow("Equalized Histogram", histImage4);



	//waitKey(0);
	//destroyAllWindows();


	////////////////////////////// CONVOLUTION ///////////////////////////////////

	//float Gauss[3][3] = {
	//						{1 / 9.0, 1 / 9.0, 1 / 9.0},
	//						{1 / 9.0, 1 / 9.0, 1 / 9.0},
	//						{1 / 9.0,1 / 9.0, 1 / 9.0}
	//};


	//Mat convImage = src.clone();
	//for (int y = 0; y < src.rows; y++)
	//	for (int x = 0; x < src.cols; x++)
	//		convImage.at<uchar>(y, x) = 0.0;

	//circularIndexing(src, convImage, Gauss);
	//namedWindow("Convolution", WINDOW_AUTOSIZE);
	//imshow("Convolution", convImage);

	//namedWindow("Source", WINDOW_AUTOSIZE);
	//imshow("Source", src);
	//waitKey(0);
	//destroyAllWindows();


	//////////////////////////// WALLIS ///////////////////////////////////

	
	Mat Wallis = WallisFunction(src,50,100,2.5,0.9);


	namedWindow("Wallis", WINDOW_AUTOSIZE);
	imshow("Wallis", Wallis);

	namedWindow("Source", WINDOW_AUTOSIZE);
	imshow("Source", src);
	
	waitKey(0);
	destroyAllWindows();
	
	return 0;
}