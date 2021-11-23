## ESP32/8266

**22 Nov 2021**

This is a wip, there is currently no support for the ESP32/8266 by
muforth.  It's a priority, but not a top priority at the moment.

## Uploading svd files

Check the files for empty elements. Remove them as per the following:
```
sed -i -e '/<description\/>/d' esp32s2.svd
```

All the svd files currently stashed here have been "sedisized".
