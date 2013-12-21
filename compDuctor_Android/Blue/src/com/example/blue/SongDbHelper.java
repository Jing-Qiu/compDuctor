package com.example.blue;

import java.util.ArrayList;
import java.util.List;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;


public class SongDbHelper {

    
    private static final String DATABASE_NAME = "PROJECT.db";

    private static final String DATABASE_TABLE = "SONGDATA";

    private static final int DATABASE_VERSION = 1;

    private static final String DATABASE_CREATE =
        "CREATE TABLE "+ DATABASE_TABLE +"(_id integer primary key autoincrement, "
            + "dir TEXT,"
            + "name TEXT,"
            + "file TEXT"
            +");";

    private SQLiteDatabase db;

    public SongDbHelper(Context ctx) {

            db = ctx.openOrCreateDatabase(DATABASE_NAME, 0, null);
            Log.i("song hepler", "database is open or created");
            
            try{
            	Cursor mCursor = db.rawQuery("SELECT * FROM " + DATABASE_TABLE, null);
        
            	if(null == mCursor) {
            		Log.i("song hepler", "database is empty");
            	}
            } catch(Exception e) {
            	
            	Log.i("song hepler", "something wrong");
            	try{
            	db.execSQL(DATABASE_CREATE);
            	} catch(Exception r) {
            		Log.i("song hepler", "cannot create");
            	}
            	
            }
            
            
            //db.execSQL(DATABASE_CREATE);
    }

    public void close() {
        db.close();
    }

    public void createRow(String dir, String name,  String file) {
        ContentValues initialValues = new ContentValues();
        initialValues.put("dir", dir);
        initialValues.put("name", name);
        initialValues.put("file", file);
        db.insert(DATABASE_TABLE, null, initialValues);
    }

    public void deleteRow(long rowId, String name) {
    	if(name != null) {
    		db.delete(DATABASE_TABLE, "name=" + name, null);
    	}
    	else {
    		db.delete(DATABASE_TABLE, "_id=" + rowId, null);
    	}
    }
    
    
    public void deleteAll() {
    	db.delete(DATABASE_TABLE,null,null);
    }

    public List<Row> fetchAllRows() {
        ArrayList<Row> ret = new ArrayList<Row>();
        try {
            Cursor c =
                db.query(DATABASE_TABLE, new String[] {
                    "_id", "dir", "code", "file"}, null, null, null, null, null);
            int numRows = c.getCount();
            c.moveToFirst();
            for (int i = 0; i < numRows; ++i) {
                Row row = new Row();
                row._Id = c.getLong(0);
                row.dir = c.getString(1);
                row.name = c.getString(2);
                row.file = c.getString(3);
                ret.add(row);
                c.moveToNext();
            }
        } catch (SQLException e) {
            Log.e("Exception on query", e.toString());
        }
        return ret;
    }
    
    public String printRow() {
    	
    	Cursor c = getAllRows();
    	c.moveToFirst();
    	int num = c.getCount();
        
    	String result = "";
           
        	for(int i = 0; i < num; i++) {
        		
        		result = result + "name: " + c.getString(2) + "\n";
        		result = result + "file: " + c.getString(3) + "\n";
        		
        	}
    	return result;
    }

    public Row fetchRow(long rowId, String name) {
        Row row;
        Cursor c = getAllRows();
        c.moveToFirst();
        
        int num = c.getCount();
        
        
        if (c.getCount() > 0) {
           
        	for(int i = 0; i < num; i++) {
        		
        		if(name == c.getString(2)) {
        			break;
        		}
        		
        	}
        	
        	row = new Row(c.getLong(0), c.getString(1), c.getString(2), c.getString(3));
        	
        } else {
        	row = new Row(-1, null, null, null);
        }
        return row;
    }

    public void updateRow(long rowId, String dir, String name, String file) {
        ContentValues args = new ContentValues();
        args.put("dir", dir);
        args.put("name", name);
        args.put("file", file);
        db.update(DATABASE_TABLE, args, "_id=" + rowId, null);
    }
    public Cursor getAllRows() {
        try {
            return db.query(DATABASE_TABLE, new String[] {
                    "_id", "dir", "name", "file"}, null, null, null, null, null);
        } catch (SQLException e) {
            Log.e("Exception on query", e.toString());
            return null;
        }
    }
    public Long getId(String name) {
    	
    	Cursor c = db.query(DATABASE_TABLE, new String[] {
        		"_id", "dir", "name", "file"}, "name=" + name, null , null, null, null);
    	return c.getLong(0);
    }
}
