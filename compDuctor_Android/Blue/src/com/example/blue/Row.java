package com.example.blue;

public class Row extends Object {
    public long _Id;
    public String dir;
    public String name;
    public String file;
    
    public Row(long arg0, String arg1, String arg2, String arg3) {
        _Id = arg0;
        dir = arg1;
        name = arg2;
        file = arg3;
    }
    public Row() {
        _Id = -1;
        dir = null;
        name = null;
        file = null;
    }
}
