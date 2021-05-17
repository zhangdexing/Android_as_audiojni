package com.ybs.audiojnilib;

import java.io.IOException;

public interface AudioInterface {

    //模块运行状态回调
    public void ModuleStatusCallbackFunc(int code);

    //唤醒状态回调
    public void WackupCallbackFunc(int status,int angle,int vol);

    //VAD结果回调
    public void VadCallbackFunc(int vadstatus);

    //音频数据回调
    public void AudioCallbackFunc(byte[] audiobuf,int len);

}
