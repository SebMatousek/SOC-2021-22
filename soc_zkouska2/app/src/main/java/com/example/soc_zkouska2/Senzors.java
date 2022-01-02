package com.example.soc_zkouska2;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

public class Senzors extends AppCompatActivity {

    @SuppressLint("SetTextI18n")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_senzors);
        TextView temp = findViewById(R.id.tvTep);
        TextView vlh = findViewById(R.id.tvVlh);
        TextView meter = findViewById(R.id.tvUjeto);
        TextView battery = findViewById(R.id.tvBat);
        TextView deb = findViewById(R.id.tvDeb);
        String[] ipAddress = {"192.168.0.100", "192.168.0.102"};
        final int[] pripoj = {0};


        Button back = findViewById(R.id.btnBackSen);
        back.setOnClickListener(view -> startActivity(new Intent(Senzors.this, MainActivity.class)));


        new Thread(new Runnable() {
            @Override
            public void run() {
                final StringBuilder builder = new StringBuilder();
                int n = 2+(ipAddress.length*2);
                int[] data = new int[n];
                int i = 0;
                for(String address : ipAddress) {
                    try {
                        String url = "http://" + address + "/info";//your website url
                        Document doc = Jsoup.connect(url).get();
                        Element body = doc.body();
                        builder.append(body.text());
                        String[] sData = (builder.toString()).split(";");
                        data[0] += Integer.parseInt(sData[0]);
                        data[1] += Integer.parseInt(sData[1]);
                        data[2+i] = Integer.parseInt(sData[2]);
                        data[3+i] = Integer.parseInt(sData[3]);
                        i += 2;
                        pripoj[0]++;
                        builder.setLength(0);
                    } catch (Exception ignored) {

                    }
                }
                data[0] /= pripoj[0];
                data[1] /= pripoj[0];
                final int[] finalData = data;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        temp.setText(String.valueOf(finalData[0]));
                        vlh.setText(String.valueOf(finalData[1]));
                        int i = 2;
                        while (i< finalData.length){
                            meter.append(finalData[i] +"  ");
                            i++;
                            battery.append(finalData[i] +"  ");
                            i++;
                        }
                    }
                });
            }
        }).start();

    }
}