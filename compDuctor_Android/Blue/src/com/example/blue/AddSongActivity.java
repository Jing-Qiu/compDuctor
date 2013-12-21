package com.example.blue;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;

public class AddSongActivity extends Activity {

	// Return Intent extra
    public static Row SONG_INFO = null;
    public static String SONG_DIR = "";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_add_song);
		
		
	}
	
	public void addSong(View view) {
		
		EditText ed1 = (EditText) findViewById(R.id.add_song_input_song_dir);
  		String dir = ed1.getText().toString();
		
		EditText ed2 = (EditText) findViewById(R.id.add_song_input_song_name);
  		String name = ed2.getText().toString();
  		
  		SONG_INFO = new Row(-1, dir, name, null);
  		
  		Intent intent = new Intent();
  		intent.putExtra(SONG_DIR, dir);
  		
        setResult(Activity.RESULT_OK, intent);
        
        finish();
	}
	
	

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.add_song, menu);
		return true;
	}

}
