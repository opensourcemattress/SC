#include <jni.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>

#include <cstring>
#include <malloc.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/operations.hpp>

extern "C"
{

std::string ConvertJString(JNIEnv *env, jstring str) {
    if (!str) return std::string("");

    const jsize len = env->GetStringUTFLength(str);
    const char *strChars = env->GetStringUTFChars(str, (jboolean *) 0);

    std::string Result(strChars, len);

    env->ReleaseStringUTFChars(str, strChars);

    return Result;
}

jint
//JNICALL
Java_com_android_camera_imageprocessor_PostProcessor_get8bitDataFromRAW10(
        JNIEnv *env,
        jobject obj /* this  */,
        jbyteArray rawBuffer,
        jbyteArray resultBuffer
) {
    jbyte* rawBuffer_jbyte = env->GetByteArrayElements(rawBuffer, NULL);
    jint rawBuffer_len = env->GetArrayLength(rawBuffer);

    jbyte* resultBuffer_jbyte = env->GetByteArrayElements(resultBuffer, NULL);
    unsigned char* result_buffer_data = (unsigned char*)resultBuffer_jbyte;

    if (rawBuffer_len % 5 != 0)
        return -1;

    unsigned char* rawBuffer_data = (unsigned char*)rawBuffer_jbyte;

    result_buffer_data[0] = rawBuffer_data[0];
    for (int i=1, j=1; i<rawBuffer_len; i++) {
        if (i % 5 == 0)
            continue;
        result_buffer_data[j] = rawBuffer_data[i];
        j++;
    }
    return 0;

}

jint
//JNICALL
Java_org_codeaurora_snapcam_filter_ClearSightImageProcessor_getCorrectedYUVData(
        JNIEnv *env,
        jobject obj /* this  */,
        jbyteArray yBuffer,
        jbyteArray vuBuffer,
        jbyteArray resultBuffer
) {
//    auto begin_all = std::chrono::high_resolution_clock::now();

    jbyte* yBuffer_jbyte = env->GetByteArrayElements(yBuffer, NULL);
    unsigned char* yBuffer_data = (unsigned char*)yBuffer_jbyte;

    jbyte* vuBuffer_jbyte = env->GetByteArrayElements(vuBuffer, NULL);
    unsigned char* vuBuffer_data = (unsigned char*)vuBuffer_jbyte;


    int imH = 3000;
    int imW = 4000;
    int offset = 32;

    int yLenght = 12095968; // 4032 * 3000 - 32
    int vuLenght = 6048032; // 4032 * 3000 + 32

    cv::Mat yCh = cv::Mat(imH, imW + offset, CV_8U); //, &yBuffer_data[0])
    memcpy(yCh.data, yBuffer_data, yLenght);

    cv::Mat vuCh = cv::Mat(imH / 2, imW + offset, CV_8U);
    memcpy(vuCh.data, vuBuffer_data, vuLenght);


    cv::Mat resY = yCh.colRange(0, imW);
    cv::Mat resY_copy;
    resY.copyTo(resY_copy);


    cv::Mat VUcropped = vuCh.colRange(0, imW);
    cv::Mat VUcropped_copy;
    VUcropped.copyTo(VUcropped_copy);

    jbyte* resultBuffer_jbyte = env->GetByteArrayElements(resultBuffer, NULL);
    unsigned char* result_buffer = (unsigned char*)resultBuffer_jbyte;

    memcpy(result_buffer, resY_copy.data, imH*imW);
    memcpy(result_buffer + imH*imW, VUcropped_copy.data, imH*imW/2);

//    std::ofstream fout;
//    fout.open("/mnt/sdcard/test_img.bin");
//    fout.write((char*)result_buffer, imH*imW+(imH*imW)/2);
//    fout.close();
    return 0;
}

jint
//JNICALL
Java_com_android_camera_imageprocessor_PostProcessor_imwriteYUVnative(
        JNIEnv *env,
        jobject obj /* this  */,
        jbyteArray yBuffer,
        jbyteArray vuBuffer,
        jstring jSavePath

) {
    auto begin_all = std::chrono::high_resolution_clock::now();


    std::string savePath = ConvertJString(env, jSavePath);
    const char* test = savePath.c_str();

    jbyte* yBuffer_jbyte = env->GetByteArrayElements(yBuffer, NULL);
    unsigned char* yBuffer_data = (unsigned char*)yBuffer_jbyte;

    jbyte* vuBuffer_jbyte = env->GetByteArrayElements(vuBuffer, NULL);
    unsigned char* vuBuffer_data = (unsigned char*)vuBuffer_jbyte;

    int imH = 3000;
    int imW = 4000;
    int offset = 32;

    int yLenght = 12095968; // 4032 * 3000 - 32
    int vuLenght = 6048032; // 4032 * 3000 + 32

    std::ofstream myfile;
    myfile.open ("/mnt/sdcard/camera.log");

    auto begin = std::chrono::high_resolution_clock::now();


    cv::Mat yCh = cv::Mat(imH, imW + offset, CV_8U); //, &yBuffer_data[0])
    memcpy(yCh.data, yBuffer_data, yLenght);
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = end - begin;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    myfile << "memcopy 1 " << ms << std::endl;
    begin = std::chrono::high_resolution_clock::now();

    cv::Mat vuCh = cv::Mat(imH / 2, imW + offset, CV_8U);

    memcpy(vuCh.data, vuBuffer_data, vuLenght);
    end = std::chrono::high_resolution_clock::now();
    dur = end - begin;
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    myfile << "memcopy 2 " << ms << std::endl;

//    byte[] yData = new byte[yLenght];
//    yBuffer.rewind();
//    yBuffer.get(yData, 0, sizeY);
    //yCh.(0,0, yData);
////        byte[] data = new byte[stride * height*3/2];
//
//    byte[] vuData = new byte[vuLenght];
////        vuBuffer.flip();
//
//    vuBuffer.rewind();
////        vuBuffer.get(vuData, sizeY, sizeVU);
//    vuBuffer.get(vuData, 0, sizeVU);
//
//    vuCh.put(0,0, vuData);
//
//
////        Mat result = new Mat(imH, imW, CvType.CV_8UC3);
//
    begin = std::chrono::high_resolution_clock::now();

    cv::Mat resY = yCh.colRange(0, imW);

    cv::Mat VUcropped = vuCh.colRange(0, imW);
    cv::Mat VUcroppedCopy;
    VUcropped.copyTo(VUcroppedCopy);
    end = std::chrono::high_resolution_clock::now();
    dur = end - begin;
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    myfile << "copy VU " << ms << std::endl;

    cv::Mat VUsmall = cv::Mat(imH/2, imW/2, CV_8UC2, VUcroppedCopy.data);
//    cv::Mat VUsmall = cv::Mat(imH/2, imW/2, CV_8UC2);

//#pragma parallel
//    for (int i=0; i<imH/2; i++)
//        for (int j=0; j+1<imW; j+=2) {
//            VUsmall.at<cv::Vec<unsigned char, 2>>(i,j/2)[0] = VUcropped.at<unsigned char>(i,j);
//            VUsmall.at<cv::Vec<unsigned char, 2>>(i,j/2)[1] = VUcropped.at<unsigned char>(i,j+1);
//        }


    cv::Mat VUfull;
    cv::resize(VUsmall, VUfull, cv::Size(imW, imH));

    cv::Mat VU_split[2];
    cv::split(VUfull, VU_split);


    begin = std::chrono::high_resolution_clock::now();
    cv::Mat resYUV;
    cv::Mat YUV_mat[3];
    YUV_mat[0] = resY;
    YUV_mat[1] = VU_split[1];
    YUV_mat[2] = VU_split[0];

    cv::merge(YUV_mat, 3, resYUV);

    cv::Mat res = cv::Mat(imH, imW, CV_8UC3);

    cv::cvtColor(resYUV, res, cv::COLOR_YUV2BGR);

    end = std::chrono::high_resolution_clock::now();
    dur = end - begin;
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    myfile << "cvtColor VU " << ms << std::endl;

    begin = std::chrono::high_resolution_clock::now();

    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);

    cv::imwrite(test, res, compression_params);
    end = std::chrono::high_resolution_clock::now();
    dur = end - begin;
    auto dur_all = end - begin_all;

    ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

    myfile << "imwrite " << ms << std::endl;
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur_all).count();

    myfile << "total " << ms << std::endl;

    myfile.close();
    return 0;
//    Imgproc.resize(VUsmall, VU, new org.opencv.core.Size(imW, imH));
//    ArrayList<Mat> ch = new ArrayList<Mat>();
//    org.opencv.core.Core.split(VU, ch);
//
//    Mat res = new Mat();
//    Mat res2 = new Mat();
//    ArrayList<Mat> test = new ArrayList<Mat>();
//    test.add(resY);
//    test.add(ch.get(1));
//    test.add(ch.get(0));
//
//    org.opencv.core.Core.merge(test, res);
//    Imgproc.cvtColor(res, res2, Imgproc.COLOR_YUV2BGR);
//    Imgcodecs.imwrite(imagePath, res2);

}
}
