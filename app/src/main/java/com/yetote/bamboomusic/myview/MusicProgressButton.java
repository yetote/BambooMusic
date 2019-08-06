package com.yetote.bamboomusic.myview;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;

import com.yetote.bamboomusic.R;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.com.yetote.bamboomusic.myview
 * @class 播放控制器
 * @time 2019/8/5 13:32
 * @change
 * @chang time
 * @class describe
 */
public class MusicProgressButton extends View {
    private int totalProgress;
    private int playState;
    private Paint circlePaint;
    private static final String TAG = "MusicProgressButton";
    public static final int STATE_PROGRESS = 0;
    public static final int STATE_PLAYING = 1;
    public static final int STATE_STOP = 2;
    private int width, height;
    private Paint statePaint;
    private Paint pausePaint;
    private Path trianglePath;
    private Path pausePath;
    int sweepAngle = 0;
    int playAngle = 0;

    public void setTotalTime(int totalProgress) {
        this.totalProgress = totalProgress;
    }

    public void setPlayAngle(int playAngle) {
        this.playAngle = playAngle;
        Log.e(TAG, "setPlayAngle: " + playAngle);
        invalidate();
    }

    private void setSweepAngle(int sweepAngle) {
        this.sweepAngle = sweepAngle;
        invalidate();
    }

    public MusicProgressButton(Context context) {
        super(context);
    }

    public MusicProgressButton(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init(context, attrs);
    }

    public MusicProgressButton(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context, attrs);
    }


    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        width = MeasureSpec.getSize(widthMeasureSpec);
        height = MeasureSpec.getSize(heightMeasureSpec);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        Log.e(TAG, "onDraw: ");
        canvas.drawCircle(width / 2, height / 2, width / 2 - 5, circlePaint);
        switch (playState) {
            case STATE_PROGRESS:
                canvas.drawLine(0.35f * width, 0.3f * height, 0.35f * width, 0.7f * height, pausePaint);
                canvas.drawLine(0.65f * width, 0.3f * height, 0.65f * width, 0.7f * height, pausePaint);
                canvas.drawArc(10, 10, width - 10, height - 10, -90, sweepAngle, false, circlePaint);
                break;
            case STATE_PLAYING:
                canvas.drawLine(0.35f * width, 0.3f * height, 0.35f * width, 0.7f * height, pausePaint);
                canvas.drawLine(0.65f * width, 0.3f * height, 0.65f * width, 0.7f * height, pausePaint);
                canvas.drawArc(10, 10, width - 10, height - 10, -90, playAngle, false, circlePaint);
                break;
            case STATE_STOP:
                trianglePath.moveTo(0.35f * width, 0.3f * height);
                trianglePath.lineTo(0.75f * width, 0.5f * height);
                trianglePath.lineTo(0.35f * width, 0.7f * height);
                trianglePath.lineTo(0.35f * width, 0.3f * height);
                canvas.drawPath(trianglePath, statePaint);
                break;
            default:
                break;

        }
    }

    private void init(Context context, AttributeSet attrs) {
        TypedArray ta = context.obtainStyledAttributes(attrs, R.styleable.MusicProgressButton);
        totalProgress = ta.getInt(R.styleable.MusicProgressButton_totalProgress, 0);
        playState = ta.getInt(R.styleable.MusicProgressButton_playState, 0);
        ta.recycle();
        circlePaint = new Paint();
        circlePaint.setColor(Color.rgb(60, 179, 113));
        circlePaint.setStyle(Paint.Style.STROKE);
        circlePaint.setStrokeWidth(5);
        circlePaint.setAntiAlias(true);

        statePaint = new Paint();
        statePaint.setColor(Color.rgb(60, 179, 113));
        statePaint.setAntiAlias(true);

        pausePaint = new Paint();
        pausePaint.setStrokeWidth(5);
        pausePaint.setColor(Color.rgb(60, 179, 113));
        pausePaint.setAntiAlias(true);

        trianglePath = new Path();
        pausePath = new Path();
    }

    private void showPreparingAnimation() {
        ObjectAnimator preparingAnimation = ObjectAnimator.ofInt(this, "sweepAngle", 0, 360);
        preparingAnimation.setDuration(3000);
        preparingAnimation.setRepeatCount(3);
        preparingAnimation.start();

        preparingAnimation.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                super.onAnimationEnd(animation);
                changeState(STATE_PLAYING);

            }
        });

    }

    private void showPlayingAnimation(int currentTime) throws Exception {
        if (totalProgress == 0) throw new Exception("总时长为0，请先调用setTotalTime方法设置总时长");
        float progress = (float) currentTime / (float) totalProgress;
        setPlayAngle((int) (progress * 360));
        if (currentTime >= totalProgress) {
            changeState(STATE_STOP);
        }
    }

    public void changeState(int state) {
        playState = state;
        switch (playState) {
            case STATE_PROGRESS:
                showPreparingAnimation();
                break;
            case STATE_PLAYING:
                new Thread(new Runnable() {
                    @Override
                    public void run() {

                        for (int i = 0; i < 200; i++) {
                            try {
                                showPlayingAnimation(i);
                                Thread.sleep(1000);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }).start();
                break;
            case STATE_STOP:
                break;
        }
        invalidate();
    }

    public int getPlayState() {
        return playState;
    }
}
