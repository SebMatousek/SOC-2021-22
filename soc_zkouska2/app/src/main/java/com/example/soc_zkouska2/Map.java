package com.example.soc_zkouska2;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.DataPointInterface;
import com.jjoe64.graphview.series.OnDataPointTapListener;
import com.jjoe64.graphview.series.PointsGraphSeries;
import com.jjoe64.graphview.series.Series;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;


public class Map extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map);
        GraphView graph = findViewById(R.id.graph);
        graph.removeAllSeries();
        graph.getViewport().setScalableY(true);
        graph.getGridLabelRenderer().setHighlightZeroLines(true);
        graph.getGridLabelRenderer().setVerticalLabelsVisible(true);

        TextView NameR = findViewById(R.id.tvRobot);
        TextView souradky = findViewById(R.id.tvSouradnice);

        String[] ipAddress = {"192.168.0.100", "192.168.0.102"};
        int[] colors = {Color.rgb(255,255,0),Color.rgb(0,255,0),Color.rgb(0,0,255),Color.rgb(255,20,147)};


        for (int i=0;i<ipAddress.length;i++){
            int finalI = i;
            new Thread(new Runnable() {
                @Override
                public void run() {
                    while (true){
                        final StringBuilder builder = new StringBuilder();
                        int[] data = new int[4];

                        try {
                            data[0]+=data[2];
                            data[1]+=data[3];
                            String url = "http://" + ipAddress[finalI] + "/xy";
                            Document doc = Jsoup.connect(url).get();
                            Element body = doc.body();
                            builder.append(body.text());
                            String[] sData = (builder.toString()).split(";");
                            data[2] = Integer.parseInt(sData[0]);
                            data[3] = Integer.parseInt(sData[1]);
                            builder.setLength(0);
                        } catch (Exception ignored) {
                        }

                        final int x = data[0]+data[2];
                        final int y = data[1]+data[3];
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                PointsGraphSeries<DataPoint> series = new PointsGraphSeries<>(new DataPoint[]{
                                    new DataPoint(x, y),
                                });
                                graph.addSeries(series);
                                series.setShape(PointsGraphSeries.Shape.POINT);
                                series.setColor(colors[finalI]);
                                series.setOnDataPointTapListener(new OnDataPointTapListener() {
                                    @SuppressLint("SetTextI18n")
                                    @Override
                                    public void onTap(Series series, DataPointInterface dataPoint) {
                                        NameR.setText("Robot:" + colors[finalI]);
                                        souradky.setText("souradnice:"+ dataPoint);
                                        Toast.makeText(getApplicationContext(), ""+dataPoint, Toast.LENGTH_SHORT).show();
                                    }
                                });
                            }
                        });
                    }
                }
            }).start();
        }
    }
}