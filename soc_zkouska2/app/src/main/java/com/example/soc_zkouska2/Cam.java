package com.example.soc_zkouska2;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;

import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ImageButton;

import org.jsoup.Jsoup;

public class Cam extends AppCompatActivity {

    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cam);
        WebView webView = (WebView) findViewById(R.id.webView);
        webView.setWebViewClient(new WebViewClient());
        String address = "192.168.0.100";
        webView.loadUrl("http://"+address+"/video");
        ImageButton btnNL = (ImageButton) findViewById(R.id.btnNL);
        ImageButton btnN = (ImageButton) findViewById(R.id.btnN);
        ImageButton btnNP = (ImageButton) findViewById(R.id.btnNP);
        ImageButton btnL = (ImageButton) findViewById(R.id.btnL);
        ImageButton btnM = (ImageButton) findViewById(R.id.btnM);
        ImageButton btnP = (ImageButton) findViewById(R.id.btnP);
        ImageButton btnDL = (ImageButton) findViewById(R.id.btnDL);
        ImageButton btnD = (ImageButton) findViewById(R.id.btnD);
        ImageButton btnDP = (ImageButton) findViewById(R.id.btnDP);



        btnNL.setOnTouchListener(new RepeatListener(400, 100, view -> new Thread(() -> {
            try {
                String url="http://"+address+"/nl";//your website url
                Jsoup.connect(url).get();
            } catch (Exception ignored) {
            }
        }).start()));


        btnN.setOnTouchListener(new RepeatListener(400, 100, view -> new Thread(() -> {
            try {
                String url="http://"+address+"/n";//your website url
                Jsoup.connect(url).get();
            } catch (Exception ignored) {
            }
        }).start()));


        btnNP.setOnTouchListener(new RepeatListener(400, 100, view -> new Thread(() -> {
            try {
                String url="http://"+address+"/np";//your website url
                Jsoup.connect(url).get();
            } catch (Exception ignored) {
            }
        }).start()));


        btnL.setOnTouchListener(new RepeatListener(400, 100, view -> new Thread(() -> {
            try {
                String url="http://"+address+"/l";//your website url
                Jsoup.connect(url).get();
            } catch (Exception ignored) {
            }
        }).start()));


        btnM.setOnTouchListener(new RepeatListener(400, 100, view -> new Thread(() -> {
            try {
                String url="http://"+address+"/m";//your website url
                Jsoup.connect(url).get();
            } catch (Exception ignored) {
            }
        }).start()));


        btnP.setOnTouchListener(new RepeatListener(400, 100, view -> new Thread(() -> {
            try {
                String url="http://"+address+"/p";//your website url
                Jsoup.connect(url).get();
            } catch (Exception ignored) {
            }
        }).start()));


        btnDL.setOnTouchListener(new RepeatListener(400, 100, view -> new Thread(() -> {
            try {
                String url="http://"+address+"/dl";//your website url
                Jsoup.connect(url).get();
            } catch (Exception ignored) {
            }
        }).start()));


        btnD.setOnTouchListener(new RepeatListener(400, 100, view -> new Thread(() -> {
            try {
                String url="http://"+address+"/d";//your website url
                Jsoup.connect(url).get();
            } catch (Exception ignored) {
            }
        }).start()));


        btnDP.setOnTouchListener(new RepeatListener(400, 100, view -> new Thread(() -> {
            try {
                String url="http://"+address+"/dp";//your website url
                Jsoup.connect(url).get();
            } catch (Exception ignored) {
            }
        }).start()));

    }
}