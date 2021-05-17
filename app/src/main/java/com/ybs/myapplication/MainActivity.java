package com.ybs.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.TextView;

import com.ybs.audiojnilib.AudioInterface;
import com.ybs.audiojnilib.AudioModule;

import android.os.Handler;
import android.widget.Toast;

import java.io.File;

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
    private TextView setvolume;
    private Button Btn_getvolume;
    private Button Btn_upvolume;
    private Button Btn_downvolume;
    private Button Btn_getangle;
    private Button Btn_clean;
    private Button Btn_PlayBack;
    private Button Btn_PlayMusic;

    //play music
    public MediaPlayer mediaPlayer;//MediaPlayer对象

    private boolean isPause=true;//是否暂停
    private File file;//要播放的文件
//    private TextView hint;//声明提示信息的文本框
    private Button Btn_Play;
    private Button Btn_Stop;
    private Button Btn_End;


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
        setvolume = ((TextView) findViewById(R.id.setvolume));;
        setvolume.setInputType(EditorInfo.TYPE_CLASS_PHONE);
        setvolume.setFocusable(true);
        setvolume.setFocusableInTouchMode(true);

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

//        Btn_PlayMusic = ((Button) findViewById(R.id.Btn_playmusic));
//        Btn_PlayMusic.setOnClickListener(ocl);

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

//        mediaPlayer.setLooping(false);
        mediaPlayer.setOnPreparedListener(onPreparedListener);
        mediaPlayer.setOnCompletionListener(onCompletionListener);




        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                mAudioModule.runuac();
            }
        });
        thread.start();
    }
    MediaPlayer.OnPreparedListener onPreparedListener = new MediaPlayer.OnPreparedListener() {
        @Override
        public void onPrepared(MediaPlayer mp) {
            Log.d("PlayMusic1","准备播放ing....");
//            mediaPlayer.setLooping(true);//循环播放
//            mp.start();
        }
    };

    //对MediaPlayer对象添加事件监听，当播放完成时重新开始音乐播放
    MediaPlayer.OnCompletionListener onCompletionListener = new MediaPlayer.OnCompletionListener() {
        @Override
        public void onCompletion(MediaPlayer mp) {

            if (!isPause) {
                Log.d("PlayMusic","Music播放完毕....");
                play(1);
            }
            else{
                Log.d("PlayMusic","wav播放完毕....");
                Btn_Play.setEnabled(true);
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
                    curvolume.setText(mAudioModule.getVol()+"");
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
                    break;
                case R.id.Btn_playback:
//                    mAudioModule.setVol(80);
                    mediaPlayer.stop();
                    isPause = true;
                    play(2);
                    Btn_Play.setEnabled(false);
                    Btn_Stop.setEnabled(false);
                    Btn_End.setEnabled(false);
                    break;
                case R.id.Btn_play: //对播放按钮进行事件监听

                    if(isPause){
                        play(1);
                        Btn_Stop.setText("暂停");
                        isPause=false;
                    }else {
                        mediaPlayer.start();
                    }
                    Btn_Stop.setEnabled(true);
                    Btn_End.setEnabled(true);
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
//                        hint.setText("继续播放音频....");
                        Log.d("PlayMusic","继续播放音频....");
                        isPause=false;
//                        Btn_Play.setEnabled(false);
                    }
                    break;
                case R.id.Btn_end: //对停止按钮添加事件监听器
                    mediaPlayer.pause();
                    mediaPlayer.stop();
//                    mediaPlayer.reset();
                    Log.d("PlayMusic","停止播放音频....");
                    isPause=true;
                    Btn_Stop.setEnabled(false);
                    Btn_End.setEnabled(false);
                    Btn_Play.setEnabled(true);
                    Btn_PlayBack.setEnabled(true);
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

    private void play(int type){//1:mp3  2:wav
        try{
            mediaPlayer.reset();//从新设置要播放的音乐
            //mediaPlayer.setDataSource(file.getAbsolutePath());
            if (type == 1) {
                mediaPlayer = MediaPlayer.create(this, R.raw.musictest);
                mediaPlayer.setOnPreparedListener(onPreparedListener);
                mediaPlayer.setOnCompletionListener(onCompletionListener);
//                mediaPlayer.prepare();//预加载音频
            }
            else if (type == 2) {
                String recordedFile = "/data/test.wav";
                File file = new File(recordedFile);
                mediaPlayer.setOnPreparedListener(onPreparedListener);
                mediaPlayer.setOnCompletionListener(onCompletionListener);
//                mediaPlayer = MediaPlayer.create(this, Uri.fromFile(file));
                mediaPlayer.setDataSource(file.getAbsolutePath());
                mediaPlayer.prepare();//预加载音频
            }
            mediaPlayer.start();//播放音乐
            Log.d("play","Music is starting");
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
                    break;
                case VAD_STATE_START:
                    tv.append("VAD开始.   ");
                    break;
                case VAD_STATE_END:
                    tv.append("VAD结束.   ");
                    break;
                case VAD_STATE_TIMEOUT:
                    tv.append("VAD超时.   ");
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
            Log.i("MainActivity", "VadCallback  vadstatus=VAD_STATE_START");
        }
        else if (vadstatus == VAD_STATE_END) {
            Log.i("MainActivity", "VadCallback  vadstatus=VAD_STATE_END");
        }
        else if (vadstatus == VAD_STATE_TIMEOUT) {
            Log.i("MainActivity", "VadCallback  vadstatus=VAD_STATE_TIMEOUT");
        }
        Message message = Message.obtain();
        message.arg1 = vadstatus;
        handler.sendMessage(message);
    }

    @Override
    public void AudioCallbackFunc(byte[] audiobuf, int len) {
        Log.i("MainActivity","AudioCallback" + len);
    }
}