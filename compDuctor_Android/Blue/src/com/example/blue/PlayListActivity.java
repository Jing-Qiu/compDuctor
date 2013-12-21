package com.example.blue;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

public class PlayListActivity extends Activity {
	
	// Return Intent extra
    public static String EXTRA_SONG = "I am a song";
    public final int REQUEST_ADD_SONG = 1;
    
    public String PATH_1 = "/EE519/";
    public String PATH_2 = ".txt";
    List<Row> db_values;
    
    public static ArrayList<String> list;
    //final ListView listview;
    public static StableArrayAdapter adapter;

	  @Override
	  protected void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.activity_play_list);

	    final ListView listview = (ListView) findViewById(R.id.listview);

	    list = new ArrayList<String>();
	    
	    Cursor c = MainActivity.dbHelper.getAllRows();
		c.moveToFirst();
	    int numRows = c.getCount();
        for (int i = 0; i < numRows; ++i) {
            list.add(c.getString(2));
            c.moveToNext();
        }
	    
	    adapter = new StableArrayAdapter(this,
	        android.R.layout.simple_list_item_1, list);
	    listview.setAdapter(adapter);
	    
	    listview.setOnItemClickListener(new AdapterView.OnItemClickListener() {

	        @Override
	        public void onItemClick(AdapterView<?> parent, final View view,
	            int position, long id) {
	        	final String item = (String) parent.getItemAtPosition(position);
	 	       
	     		Row song = MainActivity.dbHelper.fetchRow(-1, item);
		        
		        String file = song.file;
		        
		        Log.d("play list activity", item);
		        Log.d("file is", file);
		        
	            Intent intent = new Intent();
	            intent.putExtra(EXTRA_SONG, file);

	            // Set result and finish this Activity
	            setResult(Activity.RESULT_OK, intent);
	            finish();
	        }

	    });
	  }
	  
	public void addNewSong(View view) {
		Intent addIntent = new Intent(this, AddSongActivity.class);	   
		startActivityForResult(addIntent, REQUEST_ADD_SONG);	
	}
	
	public void refreshLibrary(View view) {
		
		Cursor c = MainActivity.dbHelper.getAllRows();
		c.moveToFirst();
		
		int num = c.getCount();
		
		for(int i = 0; i < num; i++) {
			
			String name = c.getString(2);
			String path = c.getString(1);
			Log.i("path to file", path);
			
     	    String file = "lalalala";
     	   
     	   try{
     		   
     		   file = getStringFromFile(PATH_1 + path + PATH_2);
     		  
     		   MainActivity.dbHelper.updateRow(c.getLong(0), path, name, file);
     		   Log.i("Play list", "file succesful");
     		   
     	   } catch(Exception e) {
     		   Log.i("Play list", "something wrong with get file");
     		   MainActivity.dbHelper.deleteRow(-1, name);
     		   list.remove(name);
     	   }
     	   
		}
			
	}


	public void backMain(View view) {
		Intent addIntent = new Intent(this, MainActivity.class);	   
		startActivityForResult(addIntent, REQUEST_ADD_SONG);	
	}
	
	public void clearSongs(View view) {
		MainActivity.dbHelper.deleteAll();
		list.clear();
 	   adapter.notifyDataSetChanged();
	}
	
	public void onActivityResult(int requestCode, int resultCode, Intent data) 
    {
     switch (requestCode) 
      {
       case REQUEST_ADD_SONG:
       	Log.d("Play List", "back to song ok!");
           if (resultCode == Activity.RESULT_OK) 
             {
        	   
        	   
        	   
        	   String directory = data.getExtras().getString(AddSongActivity.SONG_DIR);
        	   String file = "lalalala";
        	   Log.i("Play list", PATH_1 + directory + PATH_2);
        	   
        	   try{
        		   //file = getStringFromFile(AddSongActivity.SONG_INFO.dir);
        		   file = getStringFromFile(PATH_1 + directory + PATH_2);
        		   
        		   
        		   Log.d("song add file is", file);
        		   Toast.makeText(getApplicationContext(),"add a song to library", Toast.LENGTH_SHORT).show();
        		   Toast.makeText(getApplicationContext(),file, Toast.LENGTH_SHORT).show();
        		   MainActivity.dbHelper.createRow(directory, AddSongActivity.SONG_INFO.name, file);

        		   //list.add(AddSongActivity.SONG_INFO.name);
        	   } catch(Exception e) {
        		   Log.i("Play list", "something wrong with get file");
        		   Toast.makeText(getApplicationContext(),"No such song", Toast.LENGTH_SHORT).show();
        	   }
        	   
        	   
             }
           break;
      }
    }
	
	
	public static String convertStreamToString(InputStream is) throws Exception {
	    BufferedReader reader = new BufferedReader(new InputStreamReader(is));
	    StringBuilder sb = new StringBuilder();
	    String line = null;
	    while ((line = reader.readLine()) != null) {
	      sb.append(line).append("\n");
	    }
	    return sb.toString();
	}
	public static String getStringFromFile (String filePath) throws Exception {
		//File fl = new File(filePath);
	    File fl = new File(Environment.getExternalStorageDirectory() , filePath);
	    FileInputStream fin = new FileInputStream(fl);
	    String ret = convertStreamToString(fin);
	    //Make sure you close all streams.
	    fin.close();        
	    return ret;
	}

	  private class StableArrayAdapter extends ArrayAdapter<String> {

	    HashMap<String, Integer> mIdMap = new HashMap<String, Integer>();

	    public StableArrayAdapter(Context context, int textViewResourceId,
	        List<String> objects) {
	      super(context, textViewResourceId, objects);
	      for (int i = 0; i < objects.size(); ++i) {
	        mIdMap.put(objects.get(i), i);
	      }
	    }

	    @Override
	    public long getItemId(int position) {
	      String item = getItem(position);
	      return mIdMap.get(item);
	    }

	    @Override
	    public boolean hasStableIds() {
	      return true;
	    }

	  }
}
