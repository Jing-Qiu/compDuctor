package com.example.blue;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

    // Message types sent from the BluetoothChatService Handler
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;
	
    // Intent request codes
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    private static final int REQUEST_NEW_CARD = 3;
    private static final int REQUEST_SEND_SONG = 4;
	
	
    // Local Bluetooth adapter
    private BluetoothAdapter mBluetoothAdapter = null;
    // Member object for the chat services
    private BluetoothService mCardService = null;
    
    // Key names received from the BluetoothChatService Handler
    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";
    
    
    //for DATABASE!!!!!!!!!!!!!!!!!!!!!!!!!
    public static SongDbHelper dbHelper = null;
    
    String data = "0000";
    String Button_Buffer="00000000";
    String seekBar_BufferString="82";
    String messege_String=""; 
    // Layout Views  No Use
    private TextView mTitle;				//标题
    
    // Name of the connected device
    private String mConnectedDeviceName = null;
    // String buffer for outgoing messages
    private StringBuffer mOutStringBuffer;
    
	Context mContext = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		Log.e("MainActivity", "5");
		
		//get the blue tooth adapter
		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		
		//set the data base
		dbHelper = new SongDbHelper(this);
		
		Log.e("MainActivity", "6");
	       if (mBluetoothAdapter == null) {
	            Toast.makeText(this, "蓝牙不可用！", Toast.LENGTH_LONG).show();
	            Log.e("MainActivity", "3");
	            finish();
	            return;
	        }
			
	    Log.e("MainActivity", "7");
		//setupCards();
		Log.e("MainActivity", "8");
		
		
		/*
		try{
			testDB();
		} catch(Exception i) {
			Log.d("mian activity", "db test faild!");
		}
		*/
		/*
		try{
			testPath();
		} catch(Exception i) {
			Log.d("mian activity", "db test faild!");
		}
		*/
		
		
        Button bt1= (Button)findViewById(R.id.button_send);
        bt1.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View paramView) {
				// TODO Auto-generated method stub
				Toast.makeText(getApplicationContext(),"send a message", Toast.LENGTH_SHORT).show();
		   		 EditText ed1 = (EditText) findViewById(R.id.edit_message);
		   		 String s = ed1.getText().toString();
		   		 Toast.makeText(getApplicationContext(),s, Toast.LENGTH_SHORT).show();

		   		String data_str = new String(s.getBytes(),0,s.length());
		   		Toast.makeText(getApplicationContext(),data_str, Toast.LENGTH_SHORT).show();
	   	         sendByte(s.getBytes());
		        
			}
		});
        
        Log.e("MainActivity", "9");
	}
	
    @Override
    public void onStart() 
     {
        super.onStart();
        //确保蓝牙打开，然后读取名片信息显示在节目上（未打开则在返回信息处理中setupCards()）
        if (!mBluetoothAdapter.isEnabled()) 
           {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
            // Otherwise, setup the chat session
           } 
        else 
           {
            if (mCardService == null) setupCards();	//初始化ListView对象
           }
       }
	
    @Override
    public synchronized void onResume() 
      {
        super.onResume();
        // Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if (mCardService != null) 
           {
            // Only if the state is STATE_NONE, do we know that we haven't started already
            if (mCardService.getState() == BluetoothService.STATE_NONE) 
               {
                // Start the Bluetooth chat services
            	mCardService.start();
               }
           }
       }
    
    private void setupCards()
    {
    	Log.d("MainActivity","setup Cards");
      //完成显示内容后，初始化蓝牙服务对象
      mCardService = new BluetoothService(mHandler);
      // Initialize the buffer for outgoing messages
      setmOutStringBuffer(new StringBuffer(""));
  }
    
    public void onDestroy() 
    {
     super.onDestroy();
     // Stop the Bluetooth chat services
     if (mCardService != null) mCardService.stop();
    }
	
    
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_STATE_CHANGE:
            	
                switch (msg.arg1) {
                case BluetoothService.STATE_CONNECTED:
                    //mTitle.setText("R.string.title_connected_to");
                    //mTitle.append(mConnectedDeviceName);
                	Log.i("MainActivity","send back STATE_CONNECTED");
                    //mConversationArrayAdapter.clear();
                    break;
                case BluetoothService.STATE_CONNECTING:
                    //mTitle.setText("R.string.title_connecting");
                	Log.i("MainActivity","send back STATE_CONNECTING");
                    break;
                case BluetoothService.STATE_LISTEN:
                	Log.i("MainActivity","send back STATE_LISTEN");
                case BluetoothService.STATE_NONE:
                    //mTitle.setText("R.string.title_not_connected");
                	Log.i("MainActivity","send back STATE_NONE");
                    break;
                    default:
                    	Log.i("MainActivity","send back ????????");
                    	break;
                }
            	
            	Log.i("MainActivity","send back MESSAGE_STATE_CHANGE");
                break;
            
            case MESSAGE_WRITE:
            	Toast.makeText(MainActivity.this, "make a message", Toast.LENGTH_LONG);
                break;
            case MESSAGE_READ:
                // construct a string from the valid bytes in the buffer
                //String readMessage = new String(readBuf, 0, msg.arg1);
                //mConversationArrayAdapter.add(mConnectedDeviceName+":  " + readMessage);
            	byte[] readBuf = (byte[]) msg.obj;
            	Toast.makeText(getApplicationContext(), "received a message!",
                        Toast.LENGTH_SHORT).show();
            	onReceiveMess(readBuf,msg.arg1);  
                break;
            case MESSAGE_DEVICE_NAME:
                // save the connected device's name
                mConnectedDeviceName = msg.getData().getString(DEVICE_NAME);
                Toast.makeText(getApplicationContext(), "Connected to "
                               + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                break;
            case MESSAGE_TOAST:
                Toast.makeText(getApplicationContext(), msg.getData().getString(TOAST),
                               Toast.LENGTH_SHORT).show();
                break;
            }
        }
    };
    
	public void connectBt(View view) {

		
		if(!mBluetoothAdapter.isEnabled()) {
			Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
			startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
			Log.i("Connect Status", "Turn on!");
		} 
		else {
			Log.i("Connect Status", "Already truned on");
		}
	}
	
	public void sycronizeLib(View view) {

		Cursor c = dbHelper.getAllRows();
		c.moveToFirst();
		 int size = c.getCount();
		 
		 String lib = String.valueOf(size);
		 
		 for(int i = 0 ; i < size; i++) {
			 
			 lib = lib + c.getString(3);
			 lib = lib + ";";
			 c.moveToNext();
			 
		 }
		 lib = lib + "!";
		 
		 Toast.makeText(getApplicationContext(), "update the library",
                 Toast.LENGTH_SHORT).show();
		sendByte(lib.getBytes());
	}
	
	public void findDevice(View view) {
		
        Intent serverIntent = new Intent(this, DeviceListActivity.class);
        startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);	
	}
	
	public void openPlayList(View view) {
		
        Intent playIntent = new Intent(this, PlayListActivity.class);
        startActivityForResult(playIntent, REQUEST_SEND_SONG);
        Log.i("main activity", "goes in to play list");
	}
	
    public void ensureDiscoverable(View view) 
    {
    	Log.d("discoverable","here!");
     if (mBluetoothAdapter.getScanMode() != BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) 
        {
         Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
         discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
         startActivity(discoverableIntent);
         Toast.makeText(getApplicationContext(),"Device is visible to others in 300s",
                 Toast.LENGTH_SHORT).show();
        }
     else {
    	 Toast.makeText(getApplicationContext(),"Device is already visible",
                 Toast.LENGTH_SHORT).show();
     }
    }
	
	
	 public void onActivityResult(int requestCode, int resultCode, Intent data) 
     {
      switch (requestCode) 
       {
        case REQUEST_CONNECT_DEVICE:
        	Log.d("Main", "back to main ok!");
            if (resultCode == Activity.RESULT_OK) 
              {
            	Log.d("Main", "result ok!");
               // Get the device MAC address
               String address = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
               // Get the BLuetoothDevice object
               BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
               // Attempt to connect to the device
               
               Log.d("Main", "device connect ok!");
               mCardService.connect(device);
              }
            break;
         case REQUEST_ENABLE_BT:
            if (resultCode == Activity.RESULT_OK) 
               {
            	Toast.makeText(this, "Blue Tooth is On now", Toast.LENGTH_SHORT).show();
            	setupCards();
               } 
            else 
               {
                Toast.makeText(this, "Blue Tooth is not On", Toast.LENGTH_SHORT).show();
                finish();
               }
            break;
         case REQUEST_SEND_SONG:
             if (resultCode == Activity.RESULT_OK) 
                {
             		Toast.makeText(this, "Send a song", Toast.LENGTH_SHORT).show();
             		final String file = data.getExtras().getString(PlayListActivity.EXTRA_SONG);
             		
             		sendByte(file.getBytes());
     			
     			
                } 
             else 
                {
                 Toast.makeText(this, "Blue Tooth is not On", Toast.LENGTH_SHORT).show();
                 finish();
                }
             break;
       }
      
   }

    private void sendByte(byte[] data)
      {
        //检查是否已经连接
        if (mCardService.getState() != BluetoothService.STATE_CONNECTED) 
           {
            Toast.makeText(this, "No connection yet", Toast.LENGTH_SHORT).show();
            return;
           }

        byte[] send = data;
    	mCardService.write(send);
        // Reset out string buffer to zero and clear the edit text field
        mOutStringBuffer.setLength(0);
      }
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
    private void onReceiveMess(byte[] data,int len)
    {
    	//TextView tv = (TextView)findViewById(R.id.et1);
    	String data_str = new String(data,0,len);

  		//tv.setText(data_str);       
    }
	
	public void setmOutStringBuffer(StringBuffer mOutStringBuffer) {
		this.mOutStringBuffer = mOutStringBuffer;
	}


	public StringBuffer getmOutStringBuffer() {
		return mOutStringBuffer;
	}
	
	private void testDB() {
		
		//dbHelper.createRow("unkown", "test", "cheer up!");
		
		
		//Cursor c = dbHelper.getAllRows();
		//c.moveToFirst();
		//String s_tmp = c.getString(2);
		//long id = c.getInt(0);
		//Row tmp = dbHelper.fetchRow(-1, c.getString(2));
		String tmp = dbHelper.printRow();
		//TextView tv = (TextView)findViewById(R.id.et1);

  		//tv.setText(tmp);
	}
	
	private void testPath() {
		String tmp = "fail";
		try{
			tmp = PlayListActivity.getStringFromFile("/sdcard/EE519/mhall.txt");
		} catch(Exception e) {
			Toast.makeText(this, "wrong", Toast.LENGTH_SHORT).show();
		}
		
		//TextView tv = (TextView)findViewById(R.id.et1);

  		//tv.setText(tmp);
	}

}
