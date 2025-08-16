package com.gm.objectdetectionwithtensorflowdemo

import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.TextView
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat

class Dashboard : AppCompatActivity() {

    val obj_image:TextView by lazy { findViewById(R.id.obj_image) }
    val obj_camera:TextView by lazy { findViewById(R.id.obj_camera) }
    val obj_canvas:TextView by lazy { findViewById(R.id.obj_canvas) }
    val obj_grayscale:TextView by lazy { findViewById(R.id.obj_grayscale) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_dashboard)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
        obj_image.setOnClickListener(View.OnClickListener {
            startActivity(Intent(this,MainActivity::class.java))
        })

        obj_camera.setOnClickListener(View.OnClickListener {
            startActivity(Intent(this,ClassificationWithCamera::class.java))
        })
        obj_canvas.setOnClickListener(View.OnClickListener {
            startActivity(Intent(this,ObjectDetectionOnCamera::class.java))
        })
        obj_grayscale.setOnClickListener(View.OnClickListener {
            startActivity(Intent(this,GrayscaleImageConversion::class.java))
        })
    }
}