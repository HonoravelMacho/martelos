package com.martelos;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ScrollView;
import android.widget.TextView;

/** Tela companheira: mostra a versao do motor e como usar o binario no Termux. */
public class CompanionActivity extends Activity {

    static {
        try {
            System.loadLibrary("martelos");
        } catch (UnsatisfiedLinkError e) {
            // Segue sem JNI: a tela usa texto estatico.
        }
    }

    private static native String engineVersion();

    private static String versionSafe() {
        try {
            return engineVersion();
        } catch (UnsatisfiedLinkError e) {
            return "M.A.R.T.E.L.O.S. (motor nativo indisponivel)";
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        TextView tv = new TextView(this);
        tv.setTextSize(16);
        tv.setText(versionSafe()
            + "\n\nMotor de criptografia em C.\n\n"
            + "Uso completo no Android via Termux:\n"
            + "1. Baixe martelos-*-android-arm64.tar.gz da Release\n"
            + "2. Extraia e rode ./martelos no Termux\n"
            + "3. ffmpeg + libgmp via pkg install ffmpeg gmp");
        ScrollView sv = new ScrollView(this);
        sv.addView(tv);
        setContentView(sv);
    }
}
