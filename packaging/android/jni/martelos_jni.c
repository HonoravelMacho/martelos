// Ponte JNI minima do app companheiro (sem GMP: so informa a versao).
// Manter MARTELOS_VERSION igual a tag da Release (o workflow atualiza via sed).
#include <jni.h>

#define MARTELOS_VERSION "6.3.0"

JNIEXPORT jstring JNICALL
Java_com_martelos_CompanionActivity_engineVersion(JNIEnv* env, jclass cls) {
    (void)cls;
    return (*env)->NewStringUTF(env, "M.A.R.T.E.L.O.S. Motor Cripto v" MARTELOS_VERSION);
}
