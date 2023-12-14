package io.crim.android.ouicore.utils;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.app.ActivityManager.RunningTaskInfo;
import android.app.Instrumentation;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ConfigurationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ResolveInfo;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.media.AudioManager;
import android.media.FaceDetector;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Environment;
import android.os.LocaleList;
import android.os.SystemClock;
import android.telephony.TelephonyManager;
import android.text.Html;
import android.text.TextPaint;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.Surface;
import android.widget.EditText;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.reflect.Method;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.nio.CharBuffer;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.Enumeration;
import java.util.List;
import java.util.Locale;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

@SuppressWarnings("deprecation")
@SuppressLint("SimpleDateFormat")
public class AndroidTool {

    private static final String TAG = "AndroidTool";

    public static String getExternalStorage() {
        return Environment.getExternalStorageDirectory().getAbsolutePath();
    }

    public static String getMacAddress(Context context) {
        if (context == null) {
            Log.w(TAG, "getMacAddress context is null");
        }
        String macAddress = "00:00:00:00:00:00";
        try {
            WifiManager wifiMgr = (WifiManager) context
                    .getSystemService(Context.WIFI_SERVICE);
            WifiInfo info = (null == wifiMgr ? null : wifiMgr
                    .getConnectionInfo());
            if (null != info) {
                if (!TextUtils.isEmpty(info.getMacAddress()))
                    macAddress = info.getMacAddress();
                else
                    return macAddress;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return macAddress;
    }

    public static int getAvailableMemory(Context context) {
        if (context == null) {
            Log.w(TAG, "getAvailableMemory context is null");
        }
        ActivityManager activityManager = (ActivityManager) context
                .getSystemService(Context.ACTIVITY_SERVICE);
        MemoryInfo info = new MemoryInfo();
        activityManager.getMemoryInfo(info);
        return (int) (info.availMem / 1024 / 1024);
    }

    public static int getTotalMemory() {
        BufferedReader br = null;
        long initial_memory = 0;
        try {
            String path = "/proc/meminfo";
            FileReader fr = new FileReader(path);
            br = new BufferedReader(fr, 8192);
            String load = br.readLine();
            String[] arrayOfString = load.split("\\s+");
            initial_memory = Integer.valueOf(arrayOfString[1]).longValue();
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            try {
                br.close();
            } catch (Exception e) {
            }
        }
        return (int) (initial_memory / 1024);
    }

    static class CpuUsageTime {
        long totalTime = 0;
        long sysTime = 0;
        long cpuTime = 0;
    }


    public static CpuUsageTime getCpuUsageTime() {
        CpuUsageTime time = new CpuUsageTime();
        String[] cpuInfos = null;
        BufferedReader reader = null;
        InputStreamReader isReader = null;
        FileInputStream fis = null;
        try {
            fis = new FileInputStream("/proc/stat");
            isReader = new InputStreamReader(fis);
            reader = new BufferedReader(isReader, 1000);
            String load = reader.readLine();
            cpuInfos = load.split(" ");
            time.totalTime = Long.parseLong(cpuInfos[2])
                    + Long.parseLong(cpuInfos[3]) + Long.parseLong(cpuInfos[4])
                    + Long.parseLong(cpuInfos[6]) + Long.parseLong(cpuInfos[5])
                    + Long.parseLong(cpuInfos[7]) + Long.parseLong(cpuInfos[8]);

            time.sysTime = Long.parseLong(cpuInfos[2])
                    + Long.parseLong(cpuInfos[3]) + Long.parseLong(cpuInfos[4])
                    + Long.parseLong(cpuInfos[6])
                    + Long.parseLong(cpuInfos[7]) + Long.parseLong(cpuInfos[8]);
        } catch (Exception ex) {
//            CRLog.w("getCpuUsageTime ex:" + ex.getMessage());
//             ex.printStackTrace();
        } finally {
            try {
                if (reader != null) {
                    reader.close();
                }
            } catch (Exception e) {
            }
            reader = null;
            try {
                if (isReader != null) {
                    isReader.close();
                }
            } catch (Exception e) {
            }
            isReader = null;
            try {
                if (fis != null) {
                    fis.close();
                }
            } catch (Exception e) {
            }
            fis = null;
        }

        try {
            int pid = android.os.Process.myPid();
            fis = new FileInputStream("/proc/" + pid + "/stat");
            isReader = new InputStreamReader(fis);
            reader = new BufferedReader(isReader, 1000);
            String load = reader.readLine();
            cpuInfos = load.split(" ");
            time.cpuTime = Long.parseLong(cpuInfos[13])
                    + Long.parseLong(cpuInfos[14]) + Long.parseLong(cpuInfos[15])
                    + Long.parseLong(cpuInfos[16]);
        } catch (Exception ex) {
            // ex.printStackTrace();
        } finally {
            try {
                if (reader != null) {
                    reader.close();
                }
            } catch (Exception e) {
            }
            reader = null;
            try {
                if (isReader != null) {
                    isReader.close();
                }
            } catch (Exception e) {
            }
            isReader = null;
            try {
                if (fis != null) {
                    fis.close();
                }
            } catch (Exception e) {
            }
            fis = null;
        }
        return time;
    }

    public static int getMaxCpuFreq() {
        int result = 0;
        BufferedReader br = null;
        FileReader fr = null;
        try {
            String path = "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq";
            fr = new FileReader(path);
            br = new BufferedReader(fr);
            String text = br.readLine();
            result = Integer.parseInt(text);
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            try {
                if(br != null) {
                    br.close();
                }
            } catch (Exception e) {
            }
            br = null;
            try {
                if(fr != null) {
                    fr.close();
                }
            } catch (Exception e) {
            }
            fr = null;
        }
        return result;
    }

    public static String getSystemVersion() {
        return Build.VERSION.RELEASE;
    }

    public static String getSystemName() {
        StringBuffer buffer = new StringBuffer(Build.MODEL);
        buffer.append("  Android(").append(Build.VERSION.SDK_INT)
                .append(")").append(Build.VERSION.RELEASE);
        return buffer.toString();
    }

    public static int getCpuCores() {
        class CpuFilter implements FileFilter {
            public boolean accept(File pathname) {
                // Check if filename is "cpu", followed by a single digit number
                if (Pattern.matches("cpu[0-9]", pathname.getName())) {
                    return true;
                }
                return false;
            }
        }
        try {
            // Get directory containing CPU info
            File dir = new File("/sys/devices/system/cpu/");
            // Filter to only list the devices we care about
            File[] files = dir.listFiles(new CpuFilter());
            // Return the number of cores (virtual CPU devices)
            return files.length;
        } catch (Exception e) {

        }
        return 0;
    }

    public static double deviceScreenSize(Context context) {
        if (context == null) {
            Log.w(TAG, "deviceScreenSize context is null");
        }
        DisplayMetrics dm = context.getResources().getDisplayMetrics();
        double screenSize = Math.sqrt(Math.pow(dm.widthPixels, 2)
                + Math.pow(dm.heightPixels, 2))
                / (dm.density * 160);
        return screenSize;
    }

    public static long GetTimeCount() {
        return System.currentTimeMillis();
    }

    public static String getIpByName(String name) {
        try {
            InetAddress address = InetAddress.getByName(name);
            return address.getHostAddress();
        } catch (UnknownHostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return null;
    }

    public static void openUrl(Context context, String url) {
        if (context == null) {
            Log.w(TAG, "openUrl context is null");
        }
        try {
            Uri uri = Uri.parse(url);
            Intent it = new Intent(Intent.ACTION_VIEW, uri);
            it.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(it);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static String getIPAddress(Context context) {
        NetworkInfo info = ((ConnectivityManager) context
            .getSystemService(Context.CONNECTIVITY_SERVICE)).getActiveNetworkInfo();
        if (info != null && info.isConnected()) {
            if (info.getType() == ConnectivityManager.TYPE_WIFI) {
                //当前使用无线网络
                WifiManager wifiManager = (WifiManager)
                    context.getSystemService(Context.WIFI_SERVICE);
                WifiInfo wifiInfo = wifiManager.getConnectionInfo();
                String ipAddress = intIP2StringIP(wifiInfo.getIpAddress());//得到IPV4地址
                return ipAddress;
            } else if (info.getType() == ConnectivityManager.TYPE_MOBILE | info.getType() == ConnectivityManager.TYPE_ETHERNET) {
                try {
                    List<NetworkInterface> all = Collections.list(NetworkInterface
                        .getNetworkInterfaces());
                    for (NetworkInterface intf : all ) {
                        String name = intf.getName();
                        if(info.getType() == ConnectivityManager.TYPE_ETHERNET && !name.startsWith("eth")) {
                            continue;
                        }
                        if(name.startsWith("wlan")) {
                            continue;
                        }
                        for (Enumeration<InetAddress> enumIpAddr =
                             intf.getInetAddresses(); enumIpAddr.hasMoreElements(); ) {
                            InetAddress inetAddress = enumIpAddr.nextElement();
                            if (!inetAddress.isLoopbackAddress() &&
                                inetAddress instanceof Inet4Address) {
                                return inetAddress.getHostAddress();
                            }
                        }
                    }
                } catch (SocketException e) {
                    e.printStackTrace();
                }
            }
        } else {
            //当前无网络连接,请在设置中打开网络
        }
        return null;
    }

    /*public static boolean runCommand(String command) {
        ShellUtils.CommandResult rslt = ShellUtils.execCommand(command, false);
        return rslt.result == 0;
    }

    public static boolean runCommand(ArrayList<String> commands, boolean isRoot) {
        ShellUtils.CommandResult rslt = ShellUtils.execCommand(commands, isRoot);
        if (rslt.result != 0) {
            CRLog.w("runCommand result:" + rslt.result + " fail:" + rslt.errorMsg);
        }
        return rslt.result == 0;
    }

    public static String execCommand(String command, boolean isRoot) {
        ShellUtils.CommandResult rslt = ShellUtils.execCommand(command, isRoot);
        if (rslt.result == 0) {
            return rslt.successMsg;
        }
        return "";
    }

    public static boolean suCommand(String cmd) {
        ShellUtils.CommandResult rslt = ShellUtils.execCommand(cmd, true);
        return rslt.result == 0;
    }

    public static boolean suCommand(ArrayList<String> commands) {
        ShellUtils.CommandResult rslt = ShellUtils.execCommand(commands, true);
        return rslt.result == 0;
    }*/

    public static boolean postTouchEvent(int eventType, int xPos, int yPos, int metaState) {
        try {
            Instrumentation mInst = new Instrumentation();
            mInst.sendPointerSync(MotionEvent.obtain(SystemClock.uptimeMillis(),
                    SystemClock.uptimeMillis(), eventType, xPos, yPos, metaState));    //x,y 即是事件的坐标
            return true;
        } catch (Exception e) {
            e.printStackTrace();
//            CRLog.w("postTouchEvent eventType:" + eventType + " pos:" + xPos + "." + yPos + " metaState:" + metaState + " ex:" + e.getMessage());
        }
        return false;
    }

    /*public static boolean normalInstall(Context context, String filePath) {
        if (context == null) {
            Log.w(TAG, "normalInstall context is null");
        }
        try {
            runCommand("chmod 777 " + filePath);
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.setDataAndType(Uri.fromFile(new File(filePath)),
                    "application/vnd.android.package-archive");
            context.startActivity(intent);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }*/

    /*public static boolean slientInstall(Context context, String filePath,
                                        boolean restart) {
        if (context == null) {
            Log.w(TAG, "slientInstall context is null");
        }
        ArrayList<String> cmds = new ArrayList<String>();
//        cmds.add("chmod 777 " + filePath);
//        cmds.add("LD_LIBRARY_PATH=/vendor/lib:/system/lib pm install -r "
//                + filePath);
        cmds.add("pm install -r " + filePath);
        CRLog.i("slientInstall:" + filePath);
        if (restart) {
            try {
                PackageManager pm = context.getPackageManager();
                PackageInfo info = pm.getPackageArchiveInfo(filePath,
                        PackageManager.GET_ACTIVITIES);
                ApplicationInfo appInfo = info.applicationInfo;
                if (info != null) {
                    String packageName = appInfo.packageName;
                    PackageInfo packageinfo = null;
                    try {
                        packageinfo = context.getPackageManager()
                                .getPackageInfo(packageName, 0);
                    } catch (NameNotFoundException e) {
                        e.printStackTrace();
                    }
                    if (packageinfo != null) {
                        Intent resolveIntent = new Intent(Intent.ACTION_MAIN,
                                null);
                        resolveIntent.addCategory(Intent.CATEGORY_LAUNCHER);
                        resolveIntent.setPackage(packageinfo.packageName);

                        List<ResolveInfo> resolveinfoList = context
                                .getPackageManager().queryIntentActivities(
                                        resolveIntent, 0);

                        ResolveInfo resolveinfo = resolveinfoList.iterator()
                                .next();
                        if (resolveinfo != null) {
                            String className = resolveinfo.activityInfo.name;
                            cmds.add("am start -n " + packageName + "/"
                                    + className);
                        }
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        ShellUtils.CommandResult cmdRslt = ShellUtils.execCommand(cmds, true);
        boolean rslt = cmdRslt.result == 0;
        CRLog.i("slientInstall rslt:" + rslt + " errorMsg:" + cmdRslt.errorMsg + "  filePath:" + filePath);
        return rslt;
    }*/

    public static boolean isAppInstalled(Context context, String packagename) {
        PackageInfo packageInfo;
        try {
            packageInfo = context.getPackageManager().getPackageInfo(packagename, 0);
        } catch (NameNotFoundException e) {
            packageInfo = null;
            e.printStackTrace();
        }
        if(packageInfo ==null){
            //System.out.println("没有安装");
            return false;
        }else{
            //System.out.println("已经安装");
            return true;
        }
    }

    public static String getAppName(Context context) {
        if (context == null) {
            Log.w(TAG, "getAppName context is null");
            return "";
        }
        String packageName = context.getPackageName();
        String[] strs = packageName.split("\\.");
        return strs.length == 0 ? packageName : strs[strs.length - 1];
    }

    public static boolean checkAndMakeDir(File dir) {
        if (!dir.exists()) {
            dir.mkdirs();
        }
        return dir.exists();
    }

    public static String getMyDocPath(Context context) {
        if (context == null) {
            Log.w(TAG, "getMyDocPath context is null");
            return "";
        }
        File dataDir = context.getDir("cfg", Context.MODE_PRIVATE);
        if (!dataDir.exists()) {
            dataDir.mkdirs();
        }
        return dataDir.getAbsolutePath();
    }

    /**
     * 获取cache路径
     *
     * @param context
     * @return
     */
    public static String getAppCachePath(Context context) {
        if (context == null) {
            Log.w(TAG, "getAppCachePath context is null");
            return "";
        }
        if (Environment.MEDIA_MOUNTED.equals(Environment
                .getExternalStorageState())
                || !Environment.isExternalStorageRemovable()) {
            return context.getExternalCacheDir().getPath() + "/";
        } else {
            return context.getCacheDir().getPath() + "/";
        }
    }

    public static float screenDpi(Context context) {
        if (context == null) {
            Log.w(TAG, "screenDpi context is null");
        }
        DisplayMetrics dm = context.getResources().getDisplayMetrics();
        return dm.densityDpi;
    }

    public static float screenScale(Context context) {
        if (context == null) {
            Log.w(TAG, "screenScale context is null");
        }
        float scale = 1;
        try {
            DisplayMetrics dm = context.getResources().getDisplayMetrics();
            scale = dm.density;
        } catch (Exception e) {

        }
        return scale;
    }

    public static String getLanguage(Context context) {
        if (context == null) {
            Log.w(TAG, "getLanguage context is null");
        }
        Locale locale = context.getResources().getConfiguration().locale;
        return locale.toString();
    }

    public static String getVersion(Context context) {
        if (context == null) {
            Log.w(TAG, "getVersion context is null");
        }
        String version = null;
        try {
            PackageManager packageManager = context.getPackageManager();
            PackageInfo packInfo = packageManager.getPackageInfo(
                    context.getPackageName(), 0);
            version = packInfo.versionName;
        } catch (NameNotFoundException e) {
            // TODO Auto-generated catch block
            // e.printStackTrace();
        }
        return version;
    }

    public static final String MD5(String s) {
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            md.update(s.getBytes());
            byte b[] = md.digest();
            int i;

            StringBuffer buf = new StringBuffer("");
            for (int offset = 0; offset < b.length; offset++) {
                i = b[offset];
                if (i < 0)
                    i += 256;
                if (i < 16)
                    buf.append("0");
                buf.append(Integer.toHexString(i));
            }
            return buf.toString();
        } catch (NoSuchAlgorithmException e) {
            // TODO Auto-generated catch block
            // e.printStackTrace();
        }
        return null;
    }

    public static String CreateUUID() {
        return java.util.UUID.randomUUID().toString();
    }


    public static String getIpAddrMask(String ip) {
        try {
            Enumeration networkInterfaceEnumeration = NetworkInterface.getNetworkInterfaces();
            while (networkInterfaceEnumeration.hasMoreElements()) {
                NetworkInterface networkInterface = (NetworkInterface) networkInterfaceEnumeration.nextElement();
                if (!networkInterface.isUp()) {
                    continue;
                }

                for (InterfaceAddress interfaceAddress : networkInterface.getInterfaceAddresses()) {
                    if (interfaceAddress.getAddress() instanceof Inet4Address) {
                        String ipAddress =  interfaceAddress.getAddress().getHostAddress().toString();
                        if(ipAddress.equals(ip)) {
                            return calcMaskByPrefixLength(interfaceAddress.getNetworkPrefixLength());
                        }
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "get-error";
    }
    public static String getIpAddrMask() {
        try {
            Enumeration networkInterfaceEnumeration = NetworkInterface.getNetworkInterfaces();
            while (networkInterfaceEnumeration.hasMoreElements()) {
                NetworkInterface networkInterface = (NetworkInterface) networkInterfaceEnumeration.nextElement();
                if (!networkInterface.isUp()) {
                    continue;
                }

                for (InterfaceAddress interfaceAddress : networkInterface.getInterfaceAddresses()) {
                    if (interfaceAddress.getAddress() instanceof Inet4Address) {
                        String ipAddress =  interfaceAddress.getAddress().getHostAddress().toString();
                        return calcMaskByPrefixLength(interfaceAddress.getNetworkPrefixLength());
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "get-error";
    }

    public static String calcMaskByPrefixLength(int length) {
        int mask = 0xffffffff << (32 - length);
        int partsNum = 4;
        int bitsOfPart = 8;
        int[] maskParts = new int[partsNum];
        int selector = 0x000000ff;
        for (int i = 0; i < maskParts.length; i++) {
            int pos = maskParts.length - 1 - i;
            maskParts[pos] = (mask >> (i * bitsOfPart)) & selector;
        }

        String result = "";
        result = result + maskParts[0];
        for (int i = 1; i < maskParts.length; i++) {
            result = result + "." + maskParts[i];
        }
        return result;
    }

    public static String getLocalIPAddress() {
        try {
            List<NetworkInterface> all = Collections.list(NetworkInterface
                    .getNetworkInterfaces());
            for (NetworkInterface intf : all) {
                for (Enumeration<InetAddress> enumIPAddr = intf
                        .getInetAddresses(); enumIPAddr.hasMoreElements(); ) {
                    InetAddress inetAddress = enumIPAddr.nextElement();
                    if (!inetAddress.isLoopbackAddress()
                            && inetAddress instanceof Inet4Address) {
                        return inetAddress.getHostAddress().toString();
                    }
                }
            }
        } catch (Exception ex) {
        }
        return null;
    }

    public static String intIP2StringIP(int ip) {
        return (ip & 0xFF) + "." + ((ip >> 8) & 0xFF) + "."
                + ((ip >> 16) & 0xFF) + "." + (ip >> 24 & 0xFF);
    }

    public static String getCrashString() {
        StringBuffer logBuffer = new StringBuffer();
        BufferedReader br = null;
        try {
            Process process = Runtime.getRuntime().exec(
                    "logcat -v threadtime -d -s DEBUG");
            br = new BufferedReader(new InputStreamReader(
                    process.getInputStream()));

            String str = null;
            int logCount = 0;
            long time = System.currentTimeMillis();
            while ((str = br.readLine()) != null) {
                logBuffer.append(str).append("\n");
                if (System.currentTimeMillis() - time > 100) {
                    break;
                }
                if (logCount < 150) {
                    logCount++;
                } else {
                    break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                br.close();
            } catch (Exception e2) {
            }
        }
        return logBuffer.toString();
    }

    public static String throwable2String(Throwable ex) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        ex.printStackTrace(pw);
        pw.close();
        return sw.toString();
    }

    private enum NET_TYPE {NET_TYPE_UNKNOW, NET_CABLE, NET_WIFI, NET_2G, NET_3G, NET_4G, NET_5G, NET_TYPE_BUTT}

    public static int getNetType(Context context) {
        if (context == null) {
            Log.w(TAG, "getNetType context is null");
            return 0;
        }
        ConnectivityManager connectMgr = (ConnectivityManager) context
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = connectMgr.getActiveNetworkInfo();
        if (info != null && info.isAvailable() && info.isConnected()) {
            int type = info.getType();
            if (type == ConnectivityManager.TYPE_WIFI) {
                return NET_TYPE.NET_WIFI.ordinal();
            } else if (type == ConnectivityManager.TYPE_ETHERNET) {
                return NET_TYPE.NET_CABLE.ordinal();
            } else if (type == ConnectivityManager.TYPE_MOBILE) {
                switch (info.getSubtype()) {
                    case TelephonyManager.NETWORK_TYPE_CDMA:
                    case TelephonyManager.NETWORK_TYPE_GPRS:
                    case TelephonyManager.NETWORK_TYPE_EDGE:
                    case TelephonyManager.NETWORK_TYPE_1xRTT:
                    case TelephonyManager.NETWORK_TYPE_IDEN:
                        return NET_TYPE.NET_2G.ordinal();
                    case TelephonyManager.NETWORK_TYPE_UMTS:
                    case TelephonyManager.NETWORK_TYPE_EVDO_0:
                    case TelephonyManager.NETWORK_TYPE_EVDO_A:
                    case TelephonyManager.NETWORK_TYPE_HSDPA:
                    case TelephonyManager.NETWORK_TYPE_HSUPA:
                    case TelephonyManager.NETWORK_TYPE_HSPA:
                    case TelephonyManager.NETWORK_TYPE_EVDO_B:
                    case TelephonyManager.NETWORK_TYPE_EHRPD:
                    case TelephonyManager.NETWORK_TYPE_HSPAP:
                        return NET_TYPE.NET_3G.ordinal();
                    case TelephonyManager.NETWORK_TYPE_LTE:
                        return NET_TYPE.NET_4G.ordinal();
                    case TelephonyManager.NETWORK_TYPE_NR:
                        return NET_TYPE.NET_4G.ordinal();
                    default:
                        break;
                }
            } else {
                return NET_TYPE.NET_TYPE_UNKNOW.ordinal();
            }
        }
        return 0;
    }

    public static int getNetworkType(Context context) {
        if (context == null) {
            Log.w(TAG, "getNetworkType context is null");
            return 0;
        }
        ConnectivityManager connectMgr = (ConnectivityManager) context
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = connectMgr.getActiveNetworkInfo();
        if (info != null && info.isAvailable() && info.isConnected()) {
            int type = info.getType();
            if (type == ConnectivityManager.TYPE_WIFI) {
                return 1;
            } else if (type == ConnectivityManager.TYPE_ETHERNET) {
                return 2;
            } else if (type == ConnectivityManager.TYPE_MOBILE) {
                return 3;
            } else {
                return 4;
            }
        }
        return 0;
    }

    public static String getNetworkTypeStr(Context context) {
        if (context == null) {
            Log.w(TAG, "getNetworkTypeStr context is null");
            return "";
        }
        ConnectivityManager connectMgr = (ConnectivityManager) context
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = connectMgr.getActiveNetworkInfo();
        if (info != null && info.isAvailable() && info.isConnected()) {
            int type = info.getType();
            if (type == ConnectivityManager.TYPE_WIFI) {
                return "WIFI";
            }
            if (type == ConnectivityManager.TYPE_ETHERNET) {
                return "WIRED";
            } else if (type == ConnectivityManager.TYPE_MOBILE) {
                // info.getSubtype()
                // * NETWORK_TYPE_CDMA
                // * NETWORK_TYPE_EDGE
                // * NETWORK_TYPE_EVDO_0
                // * NETWORK_TYPE_EVDO_A
                // * NETWORK_TYPE_GPRS
                // * NETWORK_TYPE_HSDPA
                // * NETWORK_TYPE_HSPA
                // * NETWORK_TYPE_HSUPA
                // * NETWORK_TYPE_UMTS
                switch (info.getSubtype()) {
                    case TelephonyManager.NETWORK_TYPE_CDMA:
                    case TelephonyManager.NETWORK_TYPE_GPRS:
                    case TelephonyManager.NETWORK_TYPE_EDGE:
                    case TelephonyManager.NETWORK_TYPE_1xRTT:
                    case TelephonyManager.NETWORK_TYPE_IDEN:
                        return "2G";
                    case TelephonyManager.NETWORK_TYPE_UMTS:
                    case TelephonyManager.NETWORK_TYPE_EVDO_0:
                    case TelephonyManager.NETWORK_TYPE_EVDO_A:
                    case TelephonyManager.NETWORK_TYPE_HSDPA:
                    case TelephonyManager.NETWORK_TYPE_HSUPA:
                    case TelephonyManager.NETWORK_TYPE_HSPA:
                    case TelephonyManager.NETWORK_TYPE_EVDO_B:
                    case TelephonyManager.NETWORK_TYPE_EHRPD:
                    case TelephonyManager.NETWORK_TYPE_HSPAP:
                        return "3G";
                    case TelephonyManager.NETWORK_TYPE_LTE:
                        return "4G";
                    default:
                        break;
                }
            }
            return "unknow";
        }
        return "unAvailable";
    }

    public static void meetingLinkShare(Context context, String content,
                                        String title) {
        if (context == null) {
            Log.w(TAG, "meetingLinkShare context is null");
        }
        try {
            Intent intent = new Intent(Intent.ACTION_SEND);
            intent.setType("text/plain");
            intent.putExtra(Intent.EXTRA_TEXT, content);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            Intent shareIntent = Intent.createChooser(intent, title);
            shareIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(shareIntent);
        } catch (Exception e) {
            // e.printStackTrace();
        }
    }

    public static int GetPId() {
        return android.os.Process.myPid();
    }

    public static long GetTId() {
        return Thread.currentThread().getId();
    }

    public static void TerminateProcess(int pId, int exitCode) {
        Log.d(TAG, "TerminateProcess  pId:" + pId + "  exitCode:" + exitCode);
        android.os.Process.killProcess(pId);
    }

    public static boolean StartActivity(Context context, String packagename,
                                        String activity) {
        try {
            Log.d(TAG, "StartActivity packagename:" + packagename
                    + " activity:" + activity);
            ComponentName com = new ComponentName(packagename, activity);
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.addCategory(Intent.CATEGORY_DEFAULT);
            intent.setComponent(com);
            context.startActivity(intent);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public static boolean StartActivity(Context context, String action) {
        try {
            Log.d(TAG, "StartActivity action:" + action);
            Intent intent = new Intent(action);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(intent);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public static void StartApplicationWithPackageName(Context context,
                                                       String packagename) {
        if (context == null) {
            Log.w(TAG, "StartApplicationWithPackageName context is null");
        }
        Log.d(TAG, "StartApplicationWithPackageName " + packagename);
        try {
            PackageInfo packageinfo = null;
            try {
                packageinfo = context.getPackageManager().getPackageInfo(
                        packagename, 0);
            } catch (NameNotFoundException e) {
                e.printStackTrace();
            }
            if (packageinfo == null) {
                return;
            }

            Intent resolveIntent = new Intent(Intent.ACTION_MAIN, null);
            resolveIntent.addCategory(Intent.CATEGORY_LAUNCHER);
            resolveIntent.setPackage(packageinfo.packageName);

            List<ResolveInfo> resolveinfoList = context.getPackageManager()
                    .queryIntentActivities(resolveIntent, 0);

            ResolveInfo resolveinfo = resolveinfoList.iterator().next();
            if (resolveinfo != null) {
                String packageName = resolveinfo.activityInfo.packageName;
                String className = resolveinfo.activityInfo.name;
                // LAUNCHER Intent
                Intent intent = new Intent(Intent.ACTION_MAIN);
                intent.addCategory(Intent.CATEGORY_LAUNCHER);
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

                ComponentName cn = new ComponentName(packageName, className);

                intent.setComponent(cn);
                context.startActivity(intent);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /*public static String GetHDDInfo() {
        Log.d(TAG, "GetHDDInfo begin");
        String info = execCommand("df", true);
        int index = info.indexOf("\n");
        return info.substring(index);
    }*/

    public static boolean isAppRunning(Context context, String packageName) {
        if (context == null) {
            Log.w(TAG, "isAppRunning context is null");
        }
        try {
            ActivityManager am = (ActivityManager) context
                    .getSystemService(Context.ACTIVITY_SERVICE);
            if (Build.VERSION.SDK_INT > 20) {
                List<ActivityManager.RunningAppProcessInfo> runningProcesses = am
                        .getRunningAppProcesses();
                for (ActivityManager.RunningAppProcessInfo processInfo : runningProcesses) {
                    for (String activeProcess : processInfo.pkgList) {
                        if (activeProcess.equals(packageName)) {
                            return true;
                        }
                    }
                }
            } else {
                List<RunningTaskInfo> tasks = am.getRunningTasks(100);
                if (!tasks.isEmpty() && tasks.size() > 0) {
                    for (RunningTaskInfo taskInfo : tasks) {
                        if (taskInfo.topActivity == null
                                || taskInfo.baseActivity == null) {
                            continue;
                        }
                        if (taskInfo.topActivity.getPackageName().equals(
                                packageName)
                                && taskInfo.baseActivity.getPackageName()
                                .equals(packageName)) {
                            return true;
                        }
                    }
                }
            }
        } catch (Exception e) {
        }
        return false;
    }

    public static boolean isAppForground(Context context, String packageName) {
        if (context == null) {
            Log.w(TAG, "isAppForground context is null");
        }
        try {
            ActivityManager am = (ActivityManager) context
                    .getSystemService(Context.ACTIVITY_SERVICE);
            if (Build.VERSION.SDK_INT > 20) {
                List<ActivityManager.RunningAppProcessInfo> runningProcesses = am
                        .getRunningAppProcesses();
                for (ActivityManager.RunningAppProcessInfo processInfo : runningProcesses) {
                    if (processInfo.importance == ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND) {
                        for (String activeProcess : processInfo.pkgList) {
                            if (activeProcess.equals(packageName)) {
                                return true;
                            }
                        }
                    }
                }
            } else {
                List<RunningTaskInfo> tasks = am.getRunningTasks(1);
                if (!tasks.isEmpty() && tasks.size() > 0) {
                    ComponentName componentInfo = tasks.get(0).topActivity;
                    if (componentInfo.getPackageName().equals(packageName)) {
                        return true;
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    public static String getSystemProductVersion() {
        String value = "";
        try {
            Class<?> cls = Class.forName("android.os.SystemProperties");
            Method hideMethod = cls.getMethod("get", String.class);
            Object object = cls.newInstance();
            value = (String) hideMethod.invoke(object, "ro.product.version");
        } catch (Exception e) {
        }
        return value;
    }

    public static boolean checkApkExist(Context context, Intent intent) {
        if (context == null) {
            Log.w(TAG, "checkApkExist context is null");
        }
        List<ResolveInfo> list = context.getPackageManager()
                .queryIntentActivities(intent, 0);
        if (list.size() > 0) {
            return true;
        }
        return false;
    }

    public static boolean checkApkExist(Context context, String packageName) {
        if (context == null) {
            Log.w(TAG, "checkApkExist context is null");
        }
        if (TextUtils.isEmpty(packageName))
            return false;
        PackageInfo packageInfo = null;
        try {
            packageInfo = context.getPackageManager().getPackageInfo(
                    packageName, 0);
        } catch (NameNotFoundException e) {
            packageInfo = null;
            // e.printStackTrace();
        }
        if (packageInfo == null) {
            return false;
        } else {
            return true;
        }
    }

    public static int getResourceId(Context context, String resType,
                                    String resName) {
        if (context == null) {
            Log.w(TAG, "getResourceId context is null");
        }
        try {
            int sourceId = context.getResources().getIdentifier(resName,
                    resType, context.getPackageName());
            return sourceId;
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return 0;
    }

    public static int getResourceId(Context context, String resClassAndName) {
        if (context == null) {
            Log.w(TAG, "getResourceId context is null");
        }
        try {
            String[] strs = resClassAndName.split("\\.");
            if (strs.length == 3) {
                String resType = strs[1];
                String resName = strs[2];
                return AndroidTool.getResourceId(context, resType, resName);
            }
        } catch (Exception e) {
            // TODO Auto-generated catch block
            // e.printStackTrace();
        }
        return 0;
    }

    public static String getCurTimeStr(String foramt) {
        try {
            long time = System.currentTimeMillis();
            SimpleDateFormat format = new SimpleDateFormat(foramt);
            Date date = new Date(time);
            String str = format.format(date);
            return str;
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return "";
    }

    public static String getBootTimeStr() {
        try {
            long bootTime = System.currentTimeMillis()
                    - SystemClock.uptimeMillis();
            SimpleDateFormat format = new SimpleDateFormat(
                    "yyyy-MM-dd HH:mm:ss");
            Date date = new Date(bootTime);
            String str = format.format(date);
            return str;
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return "";
    }

    public static long getBootTime() {
        try {
            long bootTime = System.currentTimeMillis()
                    - SystemClock.uptimeMillis();
            return bootTime;
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return 0;
    }

    public static String LoadString(Context context, String resName) {
        if (context == null) {
            Log.w(TAG, "LoadString context is null");
        }
        String str = "";
        int strId = AndroidTool.getResourceId(context, "string", resName);
        if (strId > 0) {
            str = context.getResources().getString(strId);
        }
        return str;
    }

//    public static boolean openFile(Context context, String filePath) {
//        File file = new File(filePath);
//        if (!(file.exists() || file.isFile())) {
//            return false;
//        }
//        String type = MimeUtil.getMimeType(filePath);
//        if (TextUtils.isEmpty(type)) {
//            return false;
//        }
//        Intent intent = new Intent();
//        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//        intent.setAction(Intent.ACTION_VIEW);
//        intent.setDataAndType(Uri.fromFile(file), type);
//        try {
//            context.startActivity(intent);
//            return true;
//        } catch (ActivityNotFoundException e) {
//            try {
//                intent.setDataAndType(Uri.fromFile(file), "*/*");
//                context.startActivity(intent);
//                return true;
//            } catch (ActivityNotFoundException ex) {
//                ex.printStackTrace();
//            }
//        }
//        return false;
//    }

    public static String getApkVersion(Context context, String apkPath) {
        try {
            PackageManager pm = context.getPackageManager();
            PackageInfo info = pm.getPackageArchiveInfo(apkPath,
                    PackageManager.GET_ACTIVITIES);
            if (info != null) {
                return info.versionName;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static int dip2px(Context context, float dpValue) {
        float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }

    public static int px2dip(Context context, float pxValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (pxValue / scale + 0.5f);

    }

    /**
     * sp转px
     *
     * @param sp sp的值
     * @return px 像素
     */
    public static float sp2px(Context context, float sp) {
        return applyDimension(context, sp, TypedValue.COMPLEX_UNIT_SP);
    }

    /**
     * @param value 需要转换的值
     * @param unit  值的单位
     * @return px 像素
     */
    private static float applyDimension(Context context, float value, int unit) {
        return TypedValue.applyDimension(unit, value, context.getResources().getDisplayMetrics());
    }

    public static String toHexEncoding(int color) {
        String R, G, B, A;
        StringBuffer sb = new StringBuffer();
        R = Integer.toHexString(Color.red(color)).toUpperCase();
        G = Integer.toHexString(Color.green(color)).toUpperCase();
        B = Integer.toHexString(Color.blue(color)).toUpperCase();
        A = Integer.toHexString(Color.alpha(color)).toUpperCase();
        R = R.length() == 1 ? "0" + R : R;
        G = G.length() == 1 ? "0" + G : G;
        B = B.length() == 1 ? "0" + B : B;
        A = A.length() == 1 ? "0" + A : A;
        sb.append("#");
        sb.append(R);
        sb.append(G);
        sb.append(B);
        sb.append(A);
        return sb.toString();
    }

    public static Bitmap textToBitmap(CharSequence text, int txtColor,
                                      int bkColor, int fontSize, int textMargin) {
        try {
            long tickTime = SystemClock.elapsedRealtime();
            int imageHeight = fontSize + textMargin * 2;
            TextPaint paint = new TextPaint();
            paint.setTextSize(fontSize);
            int imageWidth = (int) paint.measureText(text, 0, text.length()) + 2 * textMargin;
            Bitmap bitmap = Bitmap.createBitmap(imageWidth, imageHeight, Config.ARGB_8888);
            Canvas canvas = new Canvas(bitmap);
            RectF rect = new RectF(0, 0, imageWidth, imageHeight);
            if (bkColor != 0) {
                paint.setColor(bkColor);
                canvas.drawRoundRect(rect, textMargin * 2, textMargin * 2, paint);
            }
            paint.setColor(txtColor);
            paint.setTextAlign(Align.CENTER);
            Paint.FontMetrics fontMetrics = paint.getFontMetrics();
            float top = fontMetrics.top;
            float bottom = fontMetrics.bottom;
            int baseLineY = (int) (rect.centerY() - top / 2 - bottom / 2);
            canvas.drawText(text, 0, text.length(), rect.centerX(), baseLineY, paint);
            canvas.save();
            canvas.restore();
//            CRLog.i("textToBitmap text:%s fontSize:%d textMargin:%d txtColor:%s bkColor:%s imageSize:%dx%d time:%d", text, fontSize, textMargin, toHexEncoding(txtColor), toHexEncoding(bkColor), imageWidth, imageHeight, SystemClock.elapsedRealtime() - tickTime);
            return bitmap;
        } catch (Exception e) {
//            CRLog.i("textToBitmap text:%s fontSize:%d textMargin:%d ex:%s", text, fontSize, textMargin, e.getMessage());
        }
        return null;
    }

    private static Bitmap timeStrAsBitmap(String text, int txtColor,
                                          int bkColor, int fontSize, int textMargin) {
        return textToBitmap(text, txtColor, bkColor, fontSize, textMargin);
    }

    public static Bitmap textAsBitmap(Context context, String htmlText, Rect rect) {
        int height = rect.bottom - rect.top;
        int textMargin = 6;
        return textToBitmap(Html.fromHtml(htmlText), Color.WHITE, 0, height - textMargin * 2, textMargin);
//        Bitmap bitmap = null;
//        try {
//            int width = rect.right - rect.left;
//            int height = rect.bottom - rect.top;
////            CRLog.i("textAsBitmap htmlText:%s rect:%s", htmlText, rect.toString());
//            bitmap = Bitmap.createBitmap(width, height, Config.ARGB_8888);
//            Canvas canvas = new Canvas(bitmap);
//            TextPaint paint = new TextPaint();
//            paint.setColor(Color.WHITE);
//            StaticLayout layout = new StaticLayout(Html.fromHtml(htmlText), paint, width, Layout.Alignment.ALIGN_NORMAL, 1.0f, 0.0f, false);
//            canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
//            layout.draw(canvas);
//            canvas.save();
//            canvas.restore();
////            CRLog.i("textAsBitmap htmlText:%s rect:%s bitmap:%dX%d", htmlText, rect.toString(), bitmap.getWidth(), bitmap.getHeight());
//        } catch (Exception e) {
//            CRLog.i("textAsBitmap htmlText:%s rect:%s ex:%s", htmlText, rect.toString(), e.getMessage());
//        }
//        return bitmap;
    }

    public static boolean supportOpenGLES20(Context context) {
        ActivityManager am = (ActivityManager) context
                .getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return (info.reqGlEsVersion >= 0x20000);
    }

    public static void setEditText(EditText view, String text) {
        if (view == null || text == null) {
            return;
        }
        view.setText(text);
        view.setSelection(text.length());
    }

    // 鍒ゆ柇楹﹀厠椋庢潈闄�
    /*public static boolean hasVoicePermission() {
        try {
            AudioRecord record = new AudioRecord(
                    AudioSource.VOICE_COMMUNICATION, 22050,
                    AudioFormat.CHANNEL_IN_MONO,
                    AudioFormat.ENCODING_PCM_16BIT,
                    AudioRecord.getMinBufferSize(22050,
                            AudioFormat.CHANNEL_IN_MONO,
                            AudioFormat.ENCODING_PCM_16BIT));
            record.startRecording();
            int recordingState = record.getRecordingState();
            if (recordingState == AudioRecord.RECORDSTATE_STOPPED) {
                return false;
            }
            record.release();
            return true;
        } catch (Exception e) {
            return false;
        }
    }*/

    public static boolean hasCamersPermission() {
        boolean canUse = true;
        Camera camera = null;
        try {
            camera = Camera.open();
            Camera.Parameters mParameters = camera.getParameters();
            camera.setParameters(mParameters);
        } catch (Exception e) {
            canUse = false;
        }
        if (camera != null) {
            camera.release();
        }
        return canUse;
    }

    public static Surface makePreviewSurface() {
        Surface surface = new Surface(new SurfaceTexture(10));
        Log.d(TAG, "makePreviewSurface");
        return surface;
    }

    public static boolean isWiredHeadsetConnected(Context context) {
        AudioManager am = (AudioManager) context
                .getSystemService(Context.AUDIO_SERVICE);
        int mode = AudioManager.MODE_IN_CALL;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            mode = AudioManager.MODE_IN_COMMUNICATION;
        }
        am.setMode(mode);
        boolean btConnected = am.isWiredHeadsetOn();
        Log.i(TAG, "isWiredHeadsetConnected " + btConnected);
        return btConnected;
    }

    /*public static boolean isBTHeadsetConnected(Context context) {
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        boolean btConnected = BluetoothProfile.STATE_CONNECTED == adapter
                .getProfileConnectionState(BluetoothProfile.HEADSET);
        Log.i(TAG, "isBTHeadsetConnected " + btConnected);
        return btConnected;
    }*/

    public static void setSpeakerOn(Context context, boolean speakerOut) {
        AudioManager am = (AudioManager) context
                .getSystemService(Context.AUDIO_SERVICE);
        int mode = AudioManager.MODE_IN_CALL;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            mode = AudioManager.MODE_IN_COMMUNICATION;
        }
        am.setMode(mode);
        Log.i(TAG, "setSpeakerOn " + speakerOut);
        am.setSpeakerphoneOn(speakerOut);
    }

    public static boolean getSpeakerOn(Context context) {
        AudioManager am = (AudioManager) context
                .getSystemService(Context.AUDIO_SERVICE);
        int mode = AudioManager.MODE_IN_CALL;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            mode = AudioManager.MODE_IN_COMMUNICATION;
        }
        am.setMode(mode);
        return am.isSpeakerphoneOn();
    }

    public static void setSpeakerBT(Context context, boolean speakerBT) {
        AudioManager am = (AudioManager) context
                .getSystemService(Context.AUDIO_SERVICE);
        Log.i(TAG, "setSpeakerBT " + speakerBT);
        int mode = AudioManager.MODE_IN_CALL;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
            mode = AudioManager.MODE_IN_COMMUNICATION;
        }
        am.setMode(mode);
        if (speakerBT) {
            am.startBluetoothSco();
            am.setBluetoothScoOn(true);
            am.setSpeakerphoneOn(false);
        } else {
            am.setBluetoothScoOn(false);
            am.stopBluetoothSco();
        }
    }

    public static boolean IsFileExist(String pathName) {
        try {
            File file = new File(pathName);
            return file.exists();
        } catch (Exception e) {
        }
        return false;
    }

    public static long GetFileSize(String pathName) {
        long size = 0;
        try {
            File file = new File(pathName);
            size = file.length();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return size;
    }

    public static long GetFileCreateTime(String filename) {
        try {
            File file = new File(filename);
            return file.lastModified();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return 0;
    }

    public static String AbsPath(String filename) {
        try {
            File file = new File(filename);
            return file.getAbsolutePath();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return filename;
    }

    public static boolean CopyFile(String srcFileName, String destFileName) {
        boolean rslt = false;
        FileInputStream fileInputStream = null;
        FileOutputStream fileOutputStream = null;
        try {
            File oldFile = new File(srcFileName);
            if (!oldFile.exists()) {
                return false;
            } else if (!oldFile.isFile()) {
                return false;
            } else if (!oldFile.canRead()) {
                return false;
            }

            fileInputStream = new FileInputStream(srcFileName);
            fileOutputStream = new FileOutputStream(destFileName);
            byte[] buffer = new byte[1024];
            int byteRead;
            while (-1 != (byteRead = fileInputStream.read(buffer))) {
                fileOutputStream.write(buffer, 0, byteRead);
            }
            fileOutputStream.flush();
            rslt = true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        try {
            if (fileInputStream != null) {
                fileInputStream.close();
            }
            if (fileOutputStream != null) {
                fileOutputStream.close();
            }
        } catch (Exception e) {
        }
        return rslt;
    }

    public static boolean RenameFile(String srcFileName, String destFileName) {
        try {
            File file = new File(srcFileName);
            file.renameTo(new File(destFileName));
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    public static boolean CreateDir(String path) {
        try {
            File dir = new File(path);
            if (dir.exists()) return true;
            return dir.mkdirs();
        } catch (Exception e) {
        }
        return false;
    }

    public static boolean RemoveDir(String path) {
        try {
            File dir = new File(path);
            return dir.delete();
        } catch (Exception e) {
        }
        return false;
    }

    public static Bitmap CreateARGBBitmap(int width, int height) {
        try {
            return Bitmap.createBitmap(width, height, Config.ARGB_8888);
        } catch (Exception e) {
        }
        return null;
    }

    public static Bitmap CreateRGBBitmap(int width, int height) {
        try {
            return Bitmap.createBitmap(width, height, Config.RGB_565);
        } catch (Exception e) {
        }
        return null;
    }

    public static String replaceBlank(String str) {
        String dest = "";
        if (str != null) {
            Pattern p = Pattern.compile("\\s*|\t|\r|\n");
            Matcher m = p.matcher(str);
            dest = m.replaceAll("");
        }
        return dest;
    }


    private static final int MAX_FACE = 8;

    public static Rect faceDetector(Bitmap bitmap) {
        try {
            long tickTime = SystemClock.elapsedRealtime();
            final int bitmapW = bitmap.getWidth();
            final int bitmapH = bitmap.getHeight();
            Bitmap bitmapTmp = bitmap;
            boolean copyBitmap = bitmap.getConfig() != Config.RGB_565;
            if (copyBitmap) {
                bitmapTmp = bitmap.copy(Config.RGB_565, false);
            }
            FaceDetector detector = new FaceDetector(bitmapW, bitmapH, MAX_FACE);
            FaceDetector.Face[] faces = new FaceDetector.Face[MAX_FACE];
            int findCount = detector.findFaces(bitmapTmp, faces);
            if (copyBitmap) {
                bitmapTmp.recycle();
            }

            Rect rect = null;
            float maxEyesDistance = 0;
            if (findCount > 0) {
                for (int i = 0; i < findCount; i++) {
                    FaceDetector.Face face = faces[i];
                    PointF pf = new PointF();
                    face.getMidPoint(pf);

                    float eyesDistance = face.eyesDistance();
                    if (eyesDistance <= maxEyesDistance) {
                        continue;
                    }
                    maxEyesDistance = eyesDistance;
                    int l = (int) (pf.x - eyesDistance / 2);
                    int r = (int) (pf.x + eyesDistance / 2);
                    int t = (int) (pf.y - eyesDistance / 2);
                    int b = (int) (pf.y + eyesDistance / 2);
                    rect = new Rect(l, t, r, b);
                }
            }
//            Log.i(TAG, "faceDetector rect:" + rect + " time:" + (SystemClock.elapsedRealtime() - tickTime));
            return rect;
        } catch (Exception e) {
            Log.i(TAG, "faceDetector ex:" + e.getMessage());
        }
        return null;
    }

    public static boolean createPath(String path) {
        try {
            File file = new File(path);
            if (!file.isFile()) {
                File parent = file.getParentFile();
                if (!parent.exists()) {
                    parent.mkdirs();
                }
            } else {
                file.mkdirs();
            }
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    public static long elapsedRealtime() {
        return SystemClock.elapsedRealtime();
    }

    public static void setThreadName(String name) {
        try {
            Thread.currentThread().setName(name);
        } catch (Exception e) {
        }
    }

    public static short byteArray2Short(byte[] bytes, int index) {
        short a = (short) (bytes[index] & 0xFF);
        short b = (short) (bytes[index + 1] & 0xFF);
        a <<= 8;
        return (short) (a | b);
    }

    public static void short2ByteArray(byte[] bytes, int index, short val) {
        bytes[index] = (byte) (val >> 8 & 0xFF);
        bytes[index] = (byte) (val & 0xFF);
    }

    public static final ArrayList<File> listFiles(String path) {
        ArrayList<File> files = null;
        try {
            File file = new File(path);
            File[] listFiles = file.listFiles();
            files = new ArrayList<File>();
            for (File f : listFiles) {
                files.add(f);
            }
        } catch (Exception e) {
        }
        return files;
    }

    public static double pointToLine1(int x1, int y1, int x2, int y2, int x, int y) {
        double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
        if (cross <= 0) return Math.sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));
        double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
        if (cross >= d2) return Math.sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));
        double r = cross / d2;
        double px = x1 + (x2 - x1) * r;
        double py = y1 + (y2 - y1) * r;
        return Math.sqrt((x - px) * (x - px) + (py - y1) * (py - y1));
    }

    // 点到直线的最短距离的判断 点（x0,y0） 到由两点组成的线段（x1,y1） ,( x2,y2 )
    public static double pointToLine(int x1, int y1, int x2, int y2, int x0,
                                     int y0) {
        double space = 0;
        double a, b, c;
        a = lineSpace(x1, y1, x2, y2);// 线段的长度
        b = lineSpace(x1, y1, x0, y0);// (x1,y1)到点的距离
        c = lineSpace(x2, y2, x0, y0);// (x2,y2)到点的距离
        if (c <= 0.000001 || b <= 0.000001) {
            space = 0;
            return space;
        }
        if (a <= 0.000001) {
            space = b;
            return space;
        }
        if (c * c >= a * a + b * b) {
            space = b;
            return space;
        }
        if (b * b >= a * a + c * c) {
            space = c;
            return space;
        }
        double p = (a + b + c) / 2;// 半周长
        double s = Math.sqrt(p * (p - a) * (p - b) * (p - c));// 海伦公式求面积
        space = 2 * s / a;// 返回点到线的距离（利用三角形面积公式求高）
        return space;
    }

    // 计算两点之间的距离
    private static double lineSpace(int x1, int y1, int x2, int y2) {
        double lineLength = 0;
        lineLength = Math.sqrt((x1 - x2) * (x1 - x2) + (y1 - y2)
                * (y1 - y2));
        return lineLength;
    }


    /**
     * @param china (字符串 汉字)
     * @return 字符串转GBK编码
     */
    public static String utf8ToGBK(String china) throws Exception {
        int maxCharSize = china.length() * 2;
        CharBuffer buffer = CharBuffer.allocate(maxCharSize);
        char[] arrays = china.toCharArray();
        int byteSize = 0;
        for (int i = 0; i < arrays.length; i++) {
            char ti = arrays[i];
            String strTi = Character.toString(ti);
            if (strTi.matches("[\\u4e00-\\u9fa5]")) { //匹配是否是中文
                byte[] charBytes = strTi.getBytes("GBK");
                for (byte b : charBytes) {
                    if (byteSize + 1 > maxCharSize) {
                        break;
                    }
                    buffer.put((char) b);
                    byteSize++;
                }
            } else {
                if (byteSize + 1 > maxCharSize) {
                    break;
                }
                buffer.put(ti);
                byteSize += 1;
            }
        }
        if (byteSize <= 0) {
            return "";
        }
        buffer.limit(byteSize);
        return new String(buffer.array());
    }

    public static boolean isAppBackground(Context context) {
        boolean bBackground = false;
        try {
            ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
            List<ActivityManager.RunningAppProcessInfo> appProcesses = am.getRunningAppProcesses();
            if (appProcesses == null) {
                return bBackground;
            }
            String packageName = context.getPackageName();
            for (ActivityManager.RunningAppProcessInfo appProcess : appProcesses) {
                if (appProcess.processName.equals(packageName)) {
                    if (appProcess.importance != ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND) {
                        bBackground = true;
                    } else {
                        bBackground = false;
                    }
                }
            }
        } catch (Exception e) {
        }
        return bBackground;
    }

    /**
     * 获取系统语言
     *
     * @param context
     * @return
     */
    public static Locale getSystemLocal(Context context) {
        Locale locale;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            locale = LocaleList.getDefault().get(0);
        } else {
            locale = Locale.getDefault();
        }
        return locale;
    }

    public static boolean copyAssert(Context context, String dstPath, String srcName) {
        File dstFile = new File(dstPath);
        if (dstFile.exists()) {
            return true;
        }
        InputStream inStream = null;
        FileOutputStream fos = null;
        OutputStream fs = null;
        boolean result = false;
        try {
            int byteread = 0;
            inStream = context.getResources().getAssets().open(srcName);
            fos = new FileOutputStream(dstPath);
            fs = new BufferedOutputStream(fos);
            byte[] buffer = new byte[8192];
            while ((byteread = inStream.read(buffer)) != -1) {
                fs.write(buffer, 0, byteread);
            }

            result = true;
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                inStream.close();
                fs.close();
                fos.close();
            } catch (Exception e) {
            }
        }
        return result;
    }
}
