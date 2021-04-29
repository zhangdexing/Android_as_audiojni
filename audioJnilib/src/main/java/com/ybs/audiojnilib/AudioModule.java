package com.ybs.audiojnilib;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class AudioModule {

    static {
        System.loadLibrary("audiojni-lib");
    }
    //流式数据保存
    private FileOutputStream outputStream  = null;
    private BufferedOutputStream bufferedOutputStream = null;
    //回调实体
    private AudioInterface mAudioInterface;

    //唤醒回调
    private void WackupCallback(int status,int angle){
        if(this.mAudioInterface != null)
            mAudioInterface.WackupCallbackFunc(status,angle);
    }
    //Vad回调
    private void VadCallback(int vadstatus){
        if(this.mAudioInterface != null)
            mAudioInterface.VadCallbackFunc(vadstatus);
    }
    //音频回调
    private void AudioCallback(byte[] audiobuf,int len){
        if(this.mAudioInterface != null)
            mAudioInterface.AudioCallbackFunc(audiobuf,len);
    }

    //初始化音频文件保存函数
    private void initSaveData(){
        String outFilename = "/sdcard/test_java.pcm";
        try {
            outputStream = new FileOutputStream(outFilename);
            // 获取BufferedOutputStream对象
            bufferedOutputStream = new BufferedOutputStream(outputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    //音频保存函数
    private void saveAudioData(byte[] audiobuf){
        try {
        // 往文件所在的缓冲输出流中写byte数据
        bufferedOutputStream.write(audiobuf);
        // 刷出缓冲输出流，该步很关键，要是不执行flush()方法，那么文件的内容是空的。
        bufferedOutputStream.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    //设置回调
    public void setAudioListener(AudioInterface mAudioInterface){
        this.mAudioInterface = mAudioInterface;
    }

    //获取模块版本号
    public native String getVersion();

    //启动模块
    public native int run();

    //设置音量 0-100
    public native int setVol(int vol);

    //获取音量 0-100
    public native int getVol();

}
