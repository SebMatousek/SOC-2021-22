package com.example.soc_zkouska2

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
/*      /video->kamera
        /xy -> posun souradnic
        /info -> udaje

*/
        val btnMapa =findViewById<Button>(R.id.btnMap)
        btnMapa.setOnClickListener{
            val intent = Intent(this, Map::class.java)
            startActivity(intent)
        }

        val btnSenz =findViewById<Button>(R.id.btnSenz)
        btnSenz.setOnClickListener{
            val intent = Intent(this, Senzors::class.java)
            startActivity(intent)
        }

        val btnOvl =findViewById<Button>(R.id.btnOvl)
        btnOvl.setOnClickListener{
            val intent = Intent(this, Cam::class.java)
            startActivity(intent)
        }
        val btnDebug =findViewById<Button>(R.id.btnDeb)
        btnDebug.setOnClickListener{
            val intent = Intent(this, Debug::class.java)
            startActivity(intent)
        }
    }
}