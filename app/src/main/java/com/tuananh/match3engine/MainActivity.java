package com.tuananh.match3engine;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.tuananh.match3engine.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'match3engine' library on application startup.
    static {
        System.loadLibrary("match3engine");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
    }

    /**
     * A native method that is implemented by the 'match3engine' native library,
     * which is packaged with this application.
     */
}