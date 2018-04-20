package com.cnr.ffmpeg;
import android.content.Context;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;
public class SaveFileService {

    //保存数据
    public static boolean saveFile(Context context, String username, String password){
        //创建文件对象
        //File file = new File("/data/data/cn.yzx.login", "info.txt");

        //创建文件对象 通过file目录
        //File file = new File(context.getFilesDir(),"info.txt");

        //创建文件对象 通过cache目录
        File file = new File(context.getCacheDir(), "info.txt");

        try {
            //文件输出流
            FileOutputStream fos = new FileOutputStream(file);
            //写数据
            fos.write((username + "##" + password).getBytes());

            //关闭文件流
            fos.close();
            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    //数据回显
    public static Map<String, String> getUserInfo(Context context){
        //获取文件对象
        //File file = new File("/data/data/cn.yzx.login", "info.txt");

        //获取文件对象
        //File file = new File(context.getFilesDir(), "info.txt");

        //获取文件对象
        File file = new File(context.getCacheDir(), "info.txt");

        try {
            //输入流
            FileInputStream fis = new FileInputStream(file);
            BufferedReader br = new BufferedReader(new InputStreamReader(fis));
            //读取文件中的内容
            String result = br.readLine();
            //拆分成String[] 
            String[] results = result.split("##");
            //将数据存到map集合中
            Map<String, String> userMap = new HashMap<String, String>();
            userMap.put("username", results[0]);
            userMap.put("password", results[1]);

            return userMap;
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return null;
        }

    }
}
