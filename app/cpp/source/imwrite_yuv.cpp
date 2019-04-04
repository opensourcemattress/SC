#include <jni.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <math.h>

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
Java_com_android_camera_CaptureModule_get8bitDataFromRAW10(
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
    int j = 1;
    std::ofstream fout;
    fout.open("/mnt/sdcard/result_buffer0.txt");
    fout << "test" << j << std::endl;
//    fout.write((char*)result_buffer, imH*imW+(imH*imW)/2);
    fout.close();

    for (int i=1; i<rawBuffer_len; i++) {
        if ((i+1) % 5 == 0) {
            //nothing here
        }
        else {
            result_buffer_data[j] = rawBuffer_data[i];
            j++;
        }
    }


    std::ofstream fout1;
    fout1.open("/mnt/sdcard/result_buffer.txt");
    fout1 << "test" << j << std::endl;
//    fout.write((char*)result_buffer, imH*imW+(imH*imW)/2);
    fout1.close();
    return 0;
}


jint
//JNICALL
Java_com_android_camera_imageprocessor_PostProcessor_convertAndSaveRAW10Native(
        JNIEnv *env,
        jobject obj /* this  */,
        jbyteArray rawBuffer,
        jbyteArray maskBuffer,
        jstring jSavePath) {

    std::string savePath = ConvertJString(env, jSavePath);
    const char* filename_c_str = savePath.c_str();

    int imH = 3016;
    int imW = 4032;
    const uint8_t lut[256] = {0, 3, 7, 11, 15, 19, 22, 26, 29, 33, 36, 40, 43, 47, 50, 53, 56, 59, 62, 65, 68, 71, 74, 77, 80, 82, 85, 87, 90, 92, 95, 97, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 129, 131, 133, 134, 136, 138, 139, 141, 143, 144, 146, 147, 148, 150, 151, 153, 154, 155, 157, 158, 159, 160, 161, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 182, 183, 184, 185, 186, 187, 187, 188, 189, 190, 191, 191, 192, 193, 193, 194, 195, 195, 196, 197, 197, 198, 198, 199, 200, 200, 201, 201, 202, 202, 203, 203, 204, 204, 205, 205, 205, 206, 206, 207, 207, 207, 208, 208, 208, 209, 209, 210, 210, 210, 211, 211, 211, 212, 212, 213, 213, 213, 214, 214, 214, 215, 215, 216, 216, 216, 217, 217, 217, 218, 218, 219, 219, 219, 220, 220, 221, 221, 221, 222, 222, 222, 223, 223, 224, 224, 224, 225, 225, 225, 226, 226, 227, 227, 227, 228, 228, 228, 229, 229, 230, 230, 230, 231, 231, 232, 232, 232, 233, 233, 233, 234, 234, 235, 235, 235, 236, 236, 236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 240, 241, 241, 241, 242, 242, 243, 243, 243, 244, 244, 245, 245, 245, 246, 246, 246, 247, 247, 248, 248, 248, 249, 249, 250, 250, 250, 251, 251, 251, 252, 252, 253, 253, 253, 254, 254, 255};
    jbyte* rawBuffer_jbyte = env->GetByteArrayElements(rawBuffer, NULL);
    jint rawBuffer_len = env->GetArrayLength(rawBuffer);

    if (rawBuffer_len % 5 != 0)
        return -1;
    unsigned char* rawBuffer_data = (unsigned char*)rawBuffer_jbyte;


//    jbyte* resultBuffer_jbyte = env->GetByteArrayElements(resultBuffer, NULL);
    unsigned char* result_buffer = new unsigned char[imH*imW];

    jbyte* maskBuffer_jbyte = env->GetByteArrayElements(maskBuffer, NULL);
    float* mask_data = (float*)maskBuffer_jbyte;



//     std::ofstream fout;
//    fout.open("/mnt/sdcard/result_buffer0.txt");
//    fout.write((char*)result_buffer, imH*imW+(imH*imW)/2);
//    fout.close();
        int black_level = 14;
    int count = 0;

    int j = 0;
    for (int i=0; i<rawBuffer_len; i++) {
        if ((i+1) % 5 == 0) {
            //nothing here
        }
        else {
            float val = rawBuffer_data[i] - black_level;
//            if (val < 0)
//                val = 0;

            val = std::max(val, 0.f);
            val = val * mask_data[j];
//            if (mask_data[j] < 1.0f || mask_data[j] > 5)
//            {
//                count ++;
//            }
            val = std::min(cvRound(val), 255);
            result_buffer[j] = lut[(uint8_t)val];
            j++;
        }
    }

    cv::Mat res = cv::Mat(imH, imW, CV_8U); //, &yBuffer_data[0])
    memcpy(res.data, result_buffer, imH*imW);
    cv::imwrite(filename_c_str, res);
    delete[] result_buffer;
//    std::ofstream fout1;
//    fout1.open("/mnt/sdcard/result_buffer.txt");
//    fout1 << "test" << count << std::endl;
//    fout.write((char*)result_buffer, imH*imW+(imH*imW)/2);
//    fout1.close();
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
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
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
