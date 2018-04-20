package com.cnr.ffmpeg;

import android.Manifest;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import java.util.List;

import pub.devrel.easypermissions.AppSettingsDialog;
import pub.devrel.easypermissions.EasyPermissions;

public class MainActivity extends AppCompatActivity implements EasyPermissions.PermissionCallbacks{

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    String[] perms = {Manifest.permission.RECORD_AUDIO,Manifest.permission.WRITE_EXTERNAL_STORAGE};

    private static final int RC_LOCATION_CONTACTS_PERM = 1001;

    private boolean mIsFirstReqPermission = false;

    private  String mVideoPath;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        new CopyFileFromAssets().testCopy(this);
        checkPermissions();
        String path=getFilesDir().getAbsolutePath();
        String name="testfile.mp4";

        mVideoPath = path + "/" + name;

        // Example of a call to a native method
        Button tv = (Button) findViewById(R.id.sample_text);
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                avPlayerJNI(mVideoPath);
            }
        });

    }

    private void checkPermissions() {
        if (EasyPermissions.hasPermissions(this, perms)) {
        } else {
            EasyPermissions.requestPermissions(this, getString(R.string.permission_required),
                    RC_LOCATION_CONTACTS_PERM, perms);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);

    }

    @Override
    public void onPermissionsGranted(int requestCode, @NonNull List<String> perms) {
        mIsFirstReqPermission = true;

    }
    //被拒绝的
    @Override
    public void onPermissionsDenied(int requestCode, @NonNull List<String> perms) {
        if (EasyPermissions.somePermissionPermanentlyDenied(this, perms)) {
            new AppSettingsDialog.Builder(this)
                    .setTitle("权限申请")
                    .setRationale(R.string.permission_read_phone_hint)
                    .setNegativeButton("取消").build().show();
        }
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native int avPlayerJNI(String str);
}
