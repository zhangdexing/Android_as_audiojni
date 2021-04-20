package com.ybs.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

import com.ybs.audiojnilib.AudioInterface;
import com.ybs.audiojnilib.AudioModule;

public class MainActivity extends AppCompatActivity implements AudioInterface {

    private AudioModule mAudioModule;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //应用启动前应给予录音、文件读取、文件写入权限
        mAudioModule  = new AudioModule();
        mAudioModule.getVersion();
        mAudioModule.setAudioListener(this);
        mAudioModule.setVol(10);
        mAudioModule.run();
    }

    @Override
    public void ModuleStatusCallbackFunc(int code) {
        Log.i("MainActivity","ModuleStatusCallbackFunc" + code);
    }

    @Override
    public void WackupCallbackFunc(int status, int angle) {
        Log.i("MainActivity","WackupCallback");
    }

    @Override
    public void VadCallbackFunc(int vadstatus) {
        Log.i("MainActivity","VadCallback");
    }

    @Override
    public void AudioCallbackFunc(byte[] audiobuf, int len) {
        Log.i("MainActivity","AudioCallback" + len);
    }
}