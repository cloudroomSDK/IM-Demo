package io.crim.android.ouicore.widget;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;

import com.alibaba.android.arouter.facade.annotation.Route;
import com.amap.api.location.AMapLocation;
import com.amap.api.location.AMapLocationClient;
import com.amap.api.location.AMapLocationClientOption;
import com.amap.api.location.AMapLocationListener;
import com.amap.api.maps.AMap;
import com.amap.api.maps.CameraUpdate;
import com.amap.api.maps.CameraUpdateFactory;
import com.amap.api.maps.MapView;
import com.amap.api.maps.model.BitmapDescriptorFactory;
import com.amap.api.maps.model.CameraPosition;
import com.amap.api.maps.model.LatLng;
import com.amap.api.maps.model.Marker;
import com.amap.api.maps.model.MarkerOptions;
import com.amap.api.services.core.AMapException;
import com.amap.api.services.core.LatLonPoint;
import com.amap.api.services.geocoder.GeocodeResult;
import com.amap.api.services.geocoder.GeocodeSearch;
import com.amap.api.services.geocoder.RegeocodeQuery;
import com.amap.api.services.geocoder.RegeocodeResult;

import io.crim.android.ouicore.R;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.databinding.ActivityAmapWebViewBinding;
import io.crim.android.ouicore.entity.LocationInfo;
import io.crim.android.ouicore.net.bage.GsonHel;
import io.crim.android.ouicore.utils.Routes;

/**
 * Created by zjw on 2024/2/20.
 */
@Route(path = Routes.Core.AMAP)
public class AMapWebViewActivity extends BaseActivity<BaseViewModel, ActivityAmapWebViewBinding> {

    public final static String LATITUDE = "latitude";
    public final static String LONGITUDE = "longitude";
    public final static String LOCATION_INFO = "location_info";

    private double latitude = 0f;
    private double longitude = 0f;
    private MapView mMapView = null;
    private AMap mAMap = null;
    private Marker mMarker = null;
    private AMapLocationClient mLocationClient = null;
    private LocationInfo mLocationInfo = null;
    private float curZoom = 15;
    private GeocodeSearch geocoderSearch;

    @SuppressLint("WrongConstant")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityAmapWebViewBinding.inflate(getLayoutInflater()));
        sink();
        latitude = getIntent().getDoubleExtra(LATITUDE, 0f);
        longitude = getIntent().getDoubleExtra(LONGITUDE, 0f);
        initView();
        mMapView = view.map;
        mMapView.onCreate(savedInstanceState);
        initAMap();
    }

    private void initView() {
        if (latitude == 0f) {
            view.right.setText("确定");
            view.right.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    if (mLocationInfo != null) {
                        Intent intent = new Intent();
                        intent.putExtra(LOCATION_INFO, GsonHel.toJson(mLocationInfo));
                        setResult(RESULT_OK, intent);
                        finish();
                    }
                }
            });
        } else {
            view.right.setText("");
        }
    }

    private void initAMap() {
        mAMap = mMapView.getMap();
        mAMap.setOnCameraChangeListener(new AMap.OnCameraChangeListener() {
            @Override
            public void onCameraChange(CameraPosition position) {

            }

            @Override
            public void onCameraChangeFinish(CameraPosition position) {
                latitude = position.target.latitude;
                longitude = position.target.longitude;
                curZoom = position.zoom;
                setPosition();
                // 第一个参数表示一个Latlng，第二参数表示范围多少米，第三个参数表示是火系坐标系还是GPS原生坐标系
                RegeocodeQuery query = new RegeocodeQuery(new LatLonPoint(latitude, longitude), 200, GeocodeSearch.AMAP);
                geocoderSearch.getFromLocationAsyn(query);// 设置同步逆地理编码请求
            }

        });
        if (latitude == 0f) {
            try {
                geocoderSearch = new GeocodeSearch(this);
                geocoderSearch.setOnGeocodeSearchListener(new GeocodeSearch.OnGeocodeSearchListener() {
                    @Override
                    public void onRegeocodeSearched(RegeocodeResult result, int i) {
                        if (result != null && result.getRegeocodeAddress() != null
                            && result.getRegeocodeAddress().getFormatAddress() != null) {
                            String addressName = result.getRegeocodeAddress().getFormatAddress(); // 逆转地里编码不是每次都可以得到对应地图上的poi
                            String province = result.getRegeocodeAddress().getProvince();
                            String city = result.getRegeocodeAddress().getCity();
                            String district = result.getRegeocodeAddress().getDistrict();
                            String township = result.getRegeocodeAddress().getTownship();
                            if (addressName != null && !TextUtils.isEmpty(addressName)) {
                                String aoiName = addressName;
                                aoiName = aoiName.replace(province, "");
                                aoiName = aoiName.replace(city, "");
                                aoiName = aoiName.replace(district, "");
                                aoiName = aoiName.replace(township, "");
                                setLocationInfo(aoiName, addressName);
                            }

                        }

                    }

                    @Override
                    public void onGeocodeSearched(GeocodeResult result, int i) {

                    }
                });
            } catch (AMapException e) {
                e.printStackTrace();
            }
            try {
                AMapLocationListener mLocationListener = new AMapLocationListener() {
                    @Override
                    public void onLocationChanged(AMapLocation amapLocation) {
                        if (amapLocation != null) {
                            if (amapLocation.getErrorCode() == 0) {
                                latitude = amapLocation.getLatitude();
                                longitude = amapLocation.getLongitude();
                                setPosition();
                                String address = amapLocation.getCountry() + amapLocation.getProvince() + amapLocation.getCity() + amapLocation.getDistrict() + amapLocation.getStreet() + amapLocation.getStreetNum();
                                setLocationInfo(amapLocation.getAoiName(), address);
                            } else {
                                Log.e("AmapError", "location Error, ErrCode:"
                                    + amapLocation.getErrorCode() + ", errInfo:"
                                    + amapLocation.getErrorInfo());
                                if (amapLocation.getErrorCode() == 12) {
                                    toast("请在设备的设置中开启app的定位权限");
                                }
                            }
                        }
                        if (mLocationClient != null) {
                            mLocationClient.stopLocation();
                            mLocationClient.onDestroy();
                        }

                    }
                };
                mLocationClient = new AMapLocationClient(getApplicationContext());
                mLocationClient.setLocationListener(mLocationListener);
                AMapLocationClientOption mLocationOption = null;
                mLocationOption = new AMapLocationClientOption();
                mLocationOption.setLocationPurpose(AMapLocationClientOption.AMapLocationPurpose.Transport);
                mLocationOption.setLocationMode(AMapLocationClientOption.AMapLocationMode.Hight_Accuracy);
                mLocationOption.setNeedAddress(true);
                mLocationOption.setOnceLocationLatest(true);
                if (null != mLocationClient) {
                    mLocationClient.setLocationOption(mLocationOption);
                    mLocationClient.stopLocation();
                    mLocationClient.startLocation();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            setPosition();
            drawMarkers();
            view.ivLocation.setVisibility(View.GONE);
        }
    }

    private void setPosition() {
        CameraPosition cameraPosition = new CameraPosition(new LatLng(latitude, longitude), curZoom, 0, 30);
        CameraUpdate cameraUpdate = CameraUpdateFactory.newCameraPosition(cameraPosition);
        if (mAMap != null) {
            mAMap.moveCamera(cameraUpdate);
//            drawMarkers();
        }
    }

    private void drawMarkers() {
        if (mMarker != null) {
            mMarker.remove();
        }
        MarkerOptions markerOptions = new MarkerOptions()
            .position(new LatLng(latitude, longitude))
            .icon(BitmapDescriptorFactory.fromResource(R.mipmap.ic_chat_location))
            .draggable(true);
        if (mAMap != null) {
            mMarker = mMapView.getMap().addMarker(markerOptions);
            mMarker.showInfoWindow();
        }
    }

    private void setLocationInfo(String name, String address) {
        if (mLocationInfo == null) {
            mLocationInfo = new LocationInfo();
        }
        mLocationInfo.name = name;
        mLocationInfo.addr = address;
        mLocationInfo.latitude = latitude;
        mLocationInfo.longitude = longitude;
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
    }

    public void toBack(View v) {
        setResult(RESULT_CANCELED);
        finish();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }
}
