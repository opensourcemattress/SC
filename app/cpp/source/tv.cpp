#include <jni.h>
#include <cstdlib>

extern "C"
{

jlong
//JNICALL
Java_com_android_camera_imageprocessor_ZSLQueue_tvNative(
        JNIEnv *env,
        jobject obj /* this  */,
        jbyteArray imBytesJNI,
        jint imH,
	    jint imW
) {
     long tvH = 0;
     long tvW = 0;
     
     jbyte* imBytes_jbyte = env->GetByteArrayElements(imBytesJNI, NULL);
     jint len = env->GetArrayLength(imBytesJNI);
     if (len < imH*imW)
     {
         int test = 0;
         return -1;
     }
     unsigned char* imBytes = (unsigned char*)imBytes_jbyte;


     for (int i=0; i<imH; i++) {
      	  for (int j=0; j<imW; j+=2) {
      	       tvW += std::abs((int)imBytes[i * imW + j] - (int)imBytes[i * imW + j + 1]);
      	  }
     }
     
     for (int i = 0; i < imH; i+=2) {
	  for (int j = 0; j < imW; j++) {
	       tvH += std::abs((int)imBytes[i * imW + j] - (int)imBytes[(i + 1) * imW + j]);
	  }
     }
     
     return tvH + tvW;
}
}
