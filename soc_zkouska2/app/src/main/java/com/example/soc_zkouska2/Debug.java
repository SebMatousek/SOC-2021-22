package com.example.soc_zkouska2;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

public class Debug extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_debug);
        Button btnSend = (Button) findViewById(R.id.btnSend);
        EditText edit = (EditText) findViewById(R.id.editT);
        TextView tvProgram = (TextView) findViewById(R.id.tvProgram);

        String[] ipAddress = {"192.168.0.100", "192.168.0.102"};

        btnSend.setOnClickListener(view -> {
            String text = edit.getText().toString();
            tvProgram.append(text+"\n");
            edit.setText("");
            switch (text) {
                case "ipAddress":
                    for (String address : ipAddress) {
                        tvProgram.append(address + "\n");
                    }
                    break;
                case "sensors":
                    final int[] pripoj = {0};
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            final StringBuilder builder = new StringBuilder();
                            int n = 2 + (ipAddress.length * 2);
                            int[] data = new int[n];
                            int i = 0;
                            for (String address : ipAddress) {
                                try {
                                    String url = "http://" + address + "/info";//your website url
                                    tvProgram.append(address + "\n");
                                    Document doc = Jsoup.connect(url).get();
                                    Element body = doc.body();
                                    builder.append(body.text());
                                    String[] sData = (builder.toString()).split(";");
                                    data[0] += Integer.parseInt(sData[0]);
                                    tvProgram.append("teplota :" + data[0] + "\n");
                                    data[1] += Integer.parseInt(sData[1]);
                                    tvProgram.append("vlhkost :" + data[1] + "\n");
                                    data[2 + i] = Integer.parseInt(sData[2]);
                                    tvProgram.append("ujeto  :" + data[2 + i] + "\n");
                                    data[3 + i] = Integer.parseInt(sData[3]);
                                    tvProgram.append("baterka :" + data[3 + i] + "\n");
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
                                    tvProgram.append("teplota celkove:" + finalData[0] + "\n");
                                    tvProgram.append("vlhkost celkove:" + finalData[1] + "\n");
                                }
                            });
                        }
                    }).start();
                    break;
                case "i":
                    tvProgram.append("senzory.......sensors\n");
                    tvProgram.append("vypis ipin....ipAddress\n");
                    tvProgram.append("mapa....map\n");
                    break;
                case "map":
                    break;
            }
        });
    }
}