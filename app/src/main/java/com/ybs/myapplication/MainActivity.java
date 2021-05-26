package com.ybs.myapplication;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.app.Activity;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import com.iflytek.cloud.ErrorCode;
import com.iflytek.cloud.InitListener;
import com.iflytek.cloud.RecognizerListener;
import com.iflytek.cloud.RecognizerResult;
import com.iflytek.cloud.SpeechConstant;
import com.iflytek.cloud.SpeechError;
import com.iflytek.cloud.SpeechRecognizer;
import com.iflytek.cloud.SpeechUtility;
import com.ybs.audiojnilib.AudioInterface;
import com.ybs.audiojnilib.AudioModule;

import android.os.Handler;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;

public class MainActivity extends AppCompatActivity implements AudioInterface {

    private AudioModule mAudioModule;
    private int mangle =0;
    private int mvol = 50;

    public static final int WAKE_UP = 1;
    public static final int VAD_STATE_START = 2;
    public static final int VAD_STATE_END = 3;
    public static final int VAD_STATE_TIMEOUT = 4;

    private TextView curvolume;
    private TextView curangle;
    private TextView tv;
    private TextView devicestatus;
    private TextView asrtext;

    private Button Btn_getvolume;
    private Button Btn_upvolume;
    private Button Btn_downvolume;
    private Button Btn_getangle;
    private Button Btn_clean;
    private Button Btn_PlayBack;


    //play music
    public MediaPlayer mediaPlayer;//MediaPlayer对象
    public MediaPlayer wkPlayer;//MediaPlayer对象

    private boolean isPause=false;//是否暂停
    private Button Btn_Play;
    private Button Btn_Stop;
    private Button Btn_End;

    private StringBuffer buffer = new StringBuffer();
    // 用HashMap存储听写结果
    private HashMap<String, String> mIatResults = new LinkedHashMap<String, String>();
    // 语音听写对象
    private SpeechRecognizer mIat;
    private static String TAG = "YBS";
    private String mEngineType = SpeechConstant.TYPE_CLOUD;
    private String resultType = "json";
    private String language="zh_cn";
    private SharedPreferences mSharedPreferences;
    private static int flg=0;
    private  int playcount=0;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //应用启动前应给予录音、文件读取、文件写入权限
        mAudioModule  = new AudioModule();
        mAudioModule.setAudioListener(this);

        curvolume = ((TextView) findViewById(R.id.volumetext));
        curangle = ((TextView) findViewById(R.id.angletext));
        tv = ((TextView) findViewById(R.id.TV));
        devicestatus = ((TextView) findViewById(R.id.devicestatus));

        asrtext = ((TextView) findViewById(R.id.TextView_asr));


        Btn_getvolume = ((Button) findViewById(R.id.Btn_GetVolume));
        Btn_getvolume.setOnClickListener(ocl);

        Btn_upvolume = ((Button) findViewById(R.id.Btn_UpVolume));
        Btn_upvolume.setOnClickListener(ocl);

        Btn_downvolume = ((Button) findViewById(R.id.Btn_DownVolume));
        Btn_downvolume.setOnClickListener(ocl);


        Btn_getangle = ((Button) findViewById(R.id.Btn_GetAngle));
        Btn_getangle.setOnClickListener(ocl);


        Btn_clean = ((Button) findViewById(R.id.Btn_Clean));
        Btn_clean.setOnClickListener(ocl);

        Btn_PlayBack = ((Button) findViewById(R.id.Btn_playback));
        Btn_PlayBack.setOnClickListener(ocl);

        //playmusic
        Btn_Play=(Button)findViewById(R.id.Btn_play);//播放
        Btn_Play.setOnClickListener(ocl);
        Btn_Stop=(Button)findViewById(R.id.Btn_stop);//暂停
        Btn_Stop.setOnClickListener(ocl);
        Btn_End=(Button)findViewById(R.id.Btn_end);//停止
        Btn_End.setOnClickListener(ocl);

        if(!isFileExist()){
            Btn_Play.setEnabled(false);
        }

        mediaPlayer = new MediaPlayer();
        wkPlayer = new MediaPlayer();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                mAudioModule.runuac();
            }
        });
        thread.start();
        //save file
//        initSaveData();

        //ifly start
        SpeechUtility.createUtility(this, "appid=" + getString(R.string.app_id));
        mIat = SpeechRecognizer.createRecognizer(this, mInitListener);
        mSharedPreferences = getSharedPreferences("com.iflytek.setting",
                Activity.MODE_PRIVATE);
        if (mIat == null){
            Log.d("YBS","=====================mIat == null");
        }
        StringBuffer buf = new StringBuffer();
        buf.append("当前APPID为：");
        buf.append(getString(R.string.app_id)+"\n");
        buf.append(getString(R.string.example_explain));
        Log.d("YBS",buf.toString());
        requestPermissions();
        buffer.setLength(0);
        //mResultText.setText(null);// 清空显示内容
        mIatResults.clear();
        // 设置参数
        setParam();
        Log.d("YBS","设置参数");

        // 设置音频来源为外部文件or实时音频流数据
		mIat.setParameter(SpeechConstant.AUDIO_SOURCE, "-1");
        // 也可以像以下这样直接设置音频文件路径识别（要求设置文件在sdcard上的全路径）：
//        mIat.setParameter(SpeechConstant.AUDIO_SOURCE, "-2");
//        mIat.setParameter(SpeechConstant.ASR_SOURCE_PATH, "/data/test.wav");


    }

    private void requestPermissions(){
        try {
            if (Build.VERSION.SDK_INT >= 23) {
                int permission = ActivityCompat.checkSelfPermission(this,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE);
                if(permission!= PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(this,new String[]
                            {Manifest.permission.WRITE_EXTERNAL_STORAGE,
                                    Manifest.permission.LOCATION_HARDWARE,Manifest.permission.READ_PHONE_STATE,
                                    Manifest.permission.WRITE_SETTINGS,Manifest.permission.READ_EXTERNAL_STORAGE,
                                    Manifest.permission.RECORD_AUDIO,Manifest.permission.READ_CONTACTS},0x0010);
                }

                if(permission != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(this,new String[] {
                            Manifest.permission.ACCESS_COARSE_LOCATION,
                            Manifest.permission.ACCESS_FINE_LOCATION},0x0010);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    /**
     * 初始化监听器。
     */
    private InitListener mInitListener = new InitListener() {

        @Override
        public void onInit(int code) {
            Log.d(TAG, "SpeechRecognizer init() code = " + code);
            if (code != ErrorCode.SUCCESS) {
                Log.d("初始化失败，错误码：" , code+",请点击网址https://www.xfyun.cn/document/error-code查询解决方案");
            }
        }
    };

    /**
     * 参数设置
     *
     * @return
     */
    public void setParam() {
        // 清空参数
        mIat.setParameter(SpeechConstant.PARAMS, null);

        // 设置听写引擎
        mIat.setParameter(SpeechConstant.ENGINE_TYPE, mEngineType);
        // 设置返回结果格式
        mIat.setParameter(SpeechConstant.RESULT_TYPE, resultType);


        if(language.equals("zh_cn")) {
            String lag = mSharedPreferences.getString("iat_language_preference",
                    "mandarin");
            Log.e(TAG,"language:"+language);// 设置语言
            mIat.setParameter(SpeechConstant.LANGUAGE, "zh_cn");
            // 设置语言区域
            mIat.setParameter(SpeechConstant.ACCENT, lag);
        }else {

            mIat.setParameter(SpeechConstant.LANGUAGE, language);
        }
        Log.e(TAG,"last language:"+mIat.getParameter(SpeechConstant.LANGUAGE));

        //此处用于设置dialog中不显示错误码信息
        //mIat.setParameter("view_tips_plain","false");

        // 设置语音前端点:静音超时时间，即用户多长时间不说话则当做超时处理
        mIat.setParameter(SpeechConstant.VAD_BOS, mSharedPreferences.getString("iat_vadbos_preference", "4000"));

        // 设置语音后端点:后端点静音检测时间，即用户停止说话多长时间内即认为不再输入， 自动停止录音
        mIat.setParameter(SpeechConstant.VAD_EOS, mSharedPreferences.getString("iat_vadeos_preference", "1000"));

        // 设置标点符号,设置为"0"返回结果无标点,设置为"1"返回结果有标点
        mIat.setParameter(SpeechConstant.ASR_PTT, mSharedPreferences.getString("iat_punc_preference", "1"));

        // 设置音频保存路径，保存音频格式支持pcm、wav，设置路径为sd卡请注意WRITE_EXTERNAL_STORAGE权限
        mIat.setParameter(SpeechConstant.AUDIO_FORMAT,"wav");
        mIat.setParameter(SpeechConstant.ASR_AUDIO_PATH, Environment.getExternalStorageDirectory()+"/msc/iat.wav");
    }
    /**
     * 听写监听器。
     */
    private RecognizerListener mRecognizerListener = new RecognizerListener() {

        @Override
        public void onBeginOfSpeech() {
            // 此回调表示：sdk内部录音机已经准备好了，用户可以开始语音输入
//            showTip("开始说话");
            Log.d(TAG,"开始说话");
        }

        @Override
        public void onError(SpeechError error) {
            // Tips：
            // 错误码：10118(您没有说话)，可能是录音机权限被禁，需要提示用户打开应用的录音权限。

//            showTip(error.getPlainDescription(true));
            Log.d(TAG,"您没有说话");
        }

        @Override
        public void onEndOfSpeech() {
            // 此回调表示：检测到了语音的尾端点，已经进入识别过程，不再接受语音输入
//            showTip("结束说话");
            Log.d(TAG,"结束说话");
        }


        private void printResult(RecognizerResult results) {
            String text = JsonParser.parseIatResult(results.getResultString());

            String sn = null;
            // 读取json结果中的sn字段
            try {
                JSONObject resultJson = new JSONObject(results.getResultString());
                sn = resultJson.optString("sn");
            } catch (JSONException e) {
                e.printStackTrace();
            }

            mIatResults.put(sn, text);

            StringBuffer resultBuffer = new StringBuffer();
            for (String key : mIatResults.keySet()) {
                resultBuffer.append(mIatResults.get(key));
            }

//            mResultText.setText(resultBuffer.toString());
//            mResultText.setSelection(mResultText.length());
            Log.d("YBS","======json  resultBuffer.toString()= "+resultBuffer.toString());
            asrtext.setText("ASR识别:" + resultBuffer.toString());
        }


        @Override
        public void onResult(RecognizerResult results, boolean isLast) {
            Log.d(TAG, results.getResultString());
            System.out.println(flg++);
            if (resultType.equals("json")) {

                printResult(results);

            }else if(resultType.equals("plain")) {
                buffer.append(results.getResultString());
//                mResultText.setText(buffer.toString());
//                mResultText.setSelection(mResultText.length());
                Log.d("YBS","======plain  buffer.toString()= "+buffer.toString());
            }

//            if (isLast & cyclic) {
//                // TODO 最后的结果
//                Message message = Message.obtain();
//                message.what = 0x001;
//                han.sendMessageDelayed(message,100);
//            }
        }

        @Override
        public void onVolumeChanged(int volume, byte[] data) {
//            showTip("当前正在说话，音量大小：" + volume);
            Log.d(TAG, "返回音频数据："+data.length);
        }

        @Override
        public void onEvent(int eventType, int arg1, int arg2, Bundle obj) {
            // 以下代码用于获取与云端的会话id，当业务出错时将会话id提供给技术支持人员，可用于查询会话日志，定位出错原因
            // 若使用本地能力，会话id为null
            //	if (SpeechEvent.EVENT_SESSION_ID == eventType) {
            //		String sid = obj.getString(SpeechEvent.KEY_EVENT_SESSION_ID);
            //		Log.d(TAG, "session id =" + sid);
            //	}
        }
    };

    //执行音频流识别操作
    private void executeStream() {
        buffer.setLength(0);
        //mResultText.setText(null);// 清空显示内容
        mIatResults.clear();
        // 设置参数
        setParam();
        Log.d("YBS","设置参数");
        // 设置音频来源为外部文件
//		mIat.setParameter(SpeechConstant.AUDIO_SOURCE, "-1");
        // 也可以像以下这样直接设置音频文件路径识别（要求设置文件在sdcard上的全路径）：
        mIat.setParameter(SpeechConstant.AUDIO_SOURCE, "-2");
        mIat.setParameter(SpeechConstant.ASR_SOURCE_PATH, "/data/test.wav");
        int ret = mIat.startListening(mRecognizerListener);
        if (ret != ErrorCode.SUCCESS) {
            Log.d("YBS","识别失败,错误码：\" + ret+\",请点击网址https://www.xfyun.cn/document/error-code查询解决方案");
//			showTip("识别失败,错误码：" + ret+",请点击网址https://www.xfyun.cn/document/error-code查询解决方案");
        } else {
            byte[] audioData = FucUtil.readAudioFile(this, "iattest.wav");

            if (null != audioData) {
//                showTip(getString(R.string.text_begin_recognizer));
                // 一次（也可以分多次）写入音频文件数据，数据格式必须是采样率为8KHz或16KHz（本地识别只支持16K采样率，云端都支持），
                // 位长16bit，单声道的wav或者pcm
                // 写入8KHz采样的音频时，必须先调用setParameter(SpeechConstant.SAMPLE_RATE, "8000")设置正确的采样率
                // 注：当音频过长，静音部分时长超过VAD_EOS将导致静音后面部分不能识别。
                ArrayList<byte[]> bytes = FucUtil.splitBuffer(audioData,audioData.length,audioData.length/3);
                for(int i=0;i<bytes.size();i++) {
                    mIat.writeAudio(bytes.get(i), 0, bytes.get(i).length );

                    try {
                        Thread.sleep(1000);
                    }catch(Exception e){

                    }
                }
                mIat.stopListening();
				/*mIat.writeAudio(audioData, 0, audioData.length );
				mIat.stopListening();*/
            } else {
                mIat.cancel();
//				showTip("读取音频流失败");
                Log.d("YBS","读取音频流失败");
            }
        }
    }


    //------------ifly end

    MediaPlayer.OnPreparedListener onPreparedListener = new MediaPlayer.OnPreparedListener() {
        @Override
        public void onPrepared(MediaPlayer mp) {
            if (mp == wkPlayer)
                Log.d("PlayMusic","wkPlayer准备播放wozai ing....");
            else if (mp == mediaPlayer)
                Log.d("PlayMusic","mediaPlayer 准备播放Musicing....");


        }
    };

    //对MediaPlayer对象添加事件监听，当播放完成时重新开始音乐播放
    MediaPlayer.OnCompletionListener onCompletionListener = new MediaPlayer.OnCompletionListener() {
        @Override
        public void onCompletion(MediaPlayer mp) {
            if (mp == wkPlayer) {
                Log.d("PlayMusic", "播放完毕wozai ing....");

            }
            else {
                if (!isPause) {

                    playcount++;
                    Log.d("PlayMusic", "Music播放完毕....playcount="+playcount);
//                    if (playcount > 50){
//                        Log.d("PlayMusic", "Music播放 50 ci  stop music....");
//                        mediaPlayer.stop();
//                        Log.d("PlayMusic","停止播放音频....");
//                        Btn_Stop.setEnabled(false);
//                        Btn_End.setEnabled(false);
//                        Btn_Play.setEnabled(true);
//                        Btn_PlayBack.setEnabled(true);
//                        playcount = 0;
//                    }else
                        play(1);
                } else {
                    Log.d("PlayMusic", "wav播放完毕....");
                    Btn_Play.setEnabled(true);
                }
            }
        }
    };
    @Override
    protected void onDestroy() {
        if(mediaPlayer.isPlaying()){
            mediaPlayer.stop();
        }
        mediaPlayer.release();
        mediaPlayer=null;
        super.onDestroy();
    }


    View.OnClickListener ocl = new View.OnClickListener() {
        int tmp;
        @Override
        public void onClick(View arg0) {
            // TODO Auto-generated method stub
            switch (arg0.getId()) {
                case R.id.Btn_GetVolume:
//                    curvolume.setText(mAudioModule.getVol()+"");
                    curvolume.setText(mvol+"");
                    break;
                case R.id.Btn_UpVolume:
//                    tmp = Integer.parseInt(curvolume.getText().toString());
                    tmp = mvol;
                    tmp += 5;
                    if (tmp >= 100)
                        tmp = 99;
                    if(mAudioModule.setVol(tmp) < 0)
                    {
                        Log.e("v","Btn_UpVolume  failed");
                    }else
                        mvol = tmp;
                    curvolume.setText(mvol+"");
                    break;
                case R.id.Btn_DownVolume:
//                    tmp = Integer.parseInt(curvolume.getText().toString());
                    tmp = mvol;
                    tmp -= 5;
                    if (tmp <= 0)
                        tmp = 0;
                    if(mAudioModule.setVol(tmp) < 0)
                    {
                        Log.e("v","Btn_DownVolume  failed");
                    }else
                        mvol = tmp;
                    curvolume.setText(mvol+"");
                    break;
                case R.id.Btn_GetAngle:
//                    Log.e("v","Btn_GetAngle");
                    curangle.setText(mangle+"");
                    break;
                case R.id.Btn_Clean:
                    tv.setText("状态:");
                    mIat.stopListening();
                    break;
                case R.id.Btn_playback:
//                    mAudioModule.setVol(80);
                    mediaPlayer.stop();
                    isPause = true;
                    play(2);
                    Btn_Play.setEnabled(false);
                    Btn_Stop.setEnabled(false);
                    Btn_End.setEnabled(false);
//                    executeStream();
                    break;
                case R.id.Btn_play: //对播放按钮进行事件监听

                    play(1);
                    Btn_Stop.setText("暂停");
                    isPause=false;

                    Btn_Stop.setEnabled(true);
                    Btn_End.setEnabled(true);
                    playcount = 0;
//                    Btn_PlayBack.setEnabled(false);
                    break;
                case R.id.Btn_stop://对暂停、继续按钮添加事件监听器
                    if(mediaPlayer.isPlaying()&&!isPause){
                        mediaPlayer.pause();
                        isPause=true;
                        Btn_Stop.setText("继续");
                        Log.d("PlayMusic","暂停播放音频....");
//                        Btn_Play.setEnabled(false);
                    }else{
                        mediaPlayer.start();
                        Btn_Stop.setText("暂停");
                        Log.d("PlayMusic","继续播放音频....");
                        isPause=false;
//                        Btn_Play.setEnabled(false);
                    }
                    break;
                case R.id.Btn_end: //对停止按钮添加事件监听器

                    mediaPlayer.stop();
                    Log.d("PlayMusic","停止播放音频....");
                    Btn_Stop.setEnabled(false);
                    Btn_End.setEnabled(false);
                    Btn_Play.setEnabled(true);
                    Btn_PlayBack.setEnabled(true);
                    playcount = 0;
                    break;
                default:
                    break;
            }
        }
    };

    //判断文件是否存在
    private boolean  isFileExist(){
            mediaPlayer=MediaPlayer.create(this,R.raw.musictest);
            try {

            }catch (Exception e) {
                e.printStackTrace();
            }
            Toast.makeText(this,"file exist",Toast.LENGTH_LONG).show();
            return true;
    }
    //播放音乐的方法

    private void play(int type){//1:mp3  2:wav  3.wakeup
        try{
            //mediaPlayer.setDataSource(file.getAbsolutePath());
            if (type == 1) {
                mediaPlayer.reset();//从新设置要播放的音乐
                mediaPlayer = MediaPlayer.create(this, R.raw.musictest);
                mediaPlayer.setOnPreparedListener(onPreparedListener);
                mediaPlayer.setOnCompletionListener(onCompletionListener);
                mediaPlayer.start();//播放音乐
            }
            else if (type == 2) {
                mediaPlayer.reset();//从新设置要播放的音乐
                String recordedFile = "/data/test.wav";
                File file = new File(recordedFile);
                mediaPlayer.setOnPreparedListener(onPreparedListener);
                mediaPlayer.setOnCompletionListener(onCompletionListener);
//                mediaPlayer = MediaPlayer.create(this, Uri.fromFile(file));
                mediaPlayer.setDataSource(file.getAbsolutePath());
                mediaPlayer.prepare();//预加载音频
                mediaPlayer.start();//播放音乐

            }
            else if (type == 3) {
                wkPlayer.reset();//从新设置要播放的音乐
                wkPlayer = MediaPlayer.create(this, R.raw.wozai);
                wkPlayer.setOnPreparedListener(onPreparedListener);
                wkPlayer.setOnCompletionListener(onCompletionListener);
                wkPlayer.start();
            }
//            Log.d("play","Music is starting");
        } catch (Exception e) {
            e.printStackTrace();
            Log.e("err",e.getMessage());
        }
        return ;
    }




    // handler对象，用来接收消息~
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {  //这个是发送过来的消息
            super.handleMessage(msg);
            // 处理从子线程发送过来的消息
            switch (msg.arg1)
            {
                case WAKE_UP:
                    tv.append("唤醒成功.   ");
                    curangle.setText(mangle+"");
                    curvolume.setText(mvol+"");
                    if(mediaPlayer.isPlaying()&&!isPause){
                        mediaPlayer.pause();
                        isPause=true;
                        Btn_Stop.setText("继续");
                        Log.d("PlayMusic","暂停播放音频....");
//                        Btn_Play.setEnabled(false);
                    }
                    play(3);
                    break;
                case VAD_STATE_START:
                    tv.append("VAD开始.   ");
                    break;
                case VAD_STATE_END:
                    tv.append("VAD结束.   ");
                    if (isPause) {
                        mediaPlayer.start();
                        Btn_Stop.setText("暂停");
                        Log.d("PlayMusic", "继续播放mp3....");
                        isPause = false;
                    }
                    break;
                case VAD_STATE_TIMEOUT:
                    tv.append("VAD超时.   ");
                    if (isPause) {
                        mediaPlayer.start();
                        Btn_Stop.setText("暂停");
                        Log.d("PlayMusic", "继续播放mp3....");
                        isPause = false;
                    }
                    break;
                case 5:
                    devicestatus.setText("不在线");
                    break;
                case 6:
                    devicestatus.setText("在线");
                    break;
            }

        };
    };

    @Override
    public void ModuleStatusCallbackFunc(int code) {
        Log.i("MainActivity","ModuleStatusCallbackFunc" + code);
        Message message = Message.obtain();
        message.arg1 = code;
        handler.sendMessage(message);
    }

    @Override
    public void WackupCallbackFunc(int status, int angle,int vol) {
        Log.i("MainActivity","WackupCallback  status=Wackup"+"  angle="+angle +"  vol="+vol);
        mangle = angle;
        mvol = vol;
        Message message = Message.obtain();
        message.arg1 = WAKE_UP;
        handler.sendMessage(message);
    }

    @Override
    public void VadCallbackFunc(int vadstatus) {
        if (vadstatus == VAD_STATE_START) {
            mIat.startListening(mRecognizerListener);
            Log.i("MainActivity", "VadCallback  vadstatus=VAD_STATE_START");
        }
        else if (vadstatus == VAD_STATE_END) {
//            mIat.stopListening();
            Log.i("MainActivity", "VadCallback  vadstatus=VAD_STATE_END");
        }
        else if (vadstatus == VAD_STATE_TIMEOUT) {
//            mIat.stopListening();
            Log.i("MainActivity", "VadCallback  vadstatus=VAD_STATE_TIMEOUT");
        }
        Message message = Message.obtain();
        message.arg1 = vadstatus;
        handler.sendMessage(message);
    }
//流式数据保存
    private FileOutputStream outputStream  = null;
    private BufferedOutputStream bufferedOutputStream = null;

    //初始化音频文件保存函数
    private void initSaveData(){
        String outFilename = "/sdcard/ybs_asr_16k1ch.pcm";
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
    @Override
    public void AudioCallbackFunc(byte[] audiobuf, int len) {
//        saveAudioData(audiobuf);
        Log.i("MainActivity","AudioCallback" + len);
        mIat.writeAudio(audiobuf, 0, len);

    }
}