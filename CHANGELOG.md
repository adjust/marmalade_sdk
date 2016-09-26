### Version 4.10.0 (27th September 2016)
#### Added
- Added support for iOS 10.
- Added revenue deduplication for Android platform.
- Added Changelog to the repository.
- Added possibility to set session callback and partner parameters with `adjust_AddSessionCallbackParameter` and `adjust_AddSessionPartnerParameter` methods.
- Added possibility to remove session callback and partner parameters by key with `adjust_RemoveSessionCallbackParameter` and `adjust_RemoveSessionPartnerParameter` methods.
- Added possibility to remove all session callback and partner parameters with `adjust_ResetSessionCallbackParameters` and `adjust_ResetSessionPartnerParameters` methods.
- Added new `Suppress` log level.
- Added possibility to delay initialisation of the SDK while maybe waiting to obtain some session callback or partner parameters with `delayed start` feature on adjust config instance.
- Added possibility to set user agent manually on adjust config instance.

#### Changed
- Deferred deep link info will now arrive as part of the attribution response and not as part of the answer to first session.
- Updated docs.
- Native SDKs stability updates and improvements.
- Updated native iOS SDK to version **4.10.1**.
- Updated native Android SDK to version **4.10.2**.

---

### Version 4.7.0 (4th July 2016)
#### Added
- Added support for iOS iAd v3.
- Added `Bitcode` support for iOS framework.
- Added a method for getting `IDFA` on iOS device.
- Added a method for getting `Google Play Services Ad Id` on Android device.
- Added an option for enabling/disabling tracking while app is in background.
- Added a callback to be triggered if event is successfully tracked.
- Added a callback callback to be triggered if event tracking failed.
- Added a callback to be triggered if session is successfully tracked.
- Added a callback to be triggered if session tracking failed.
- Added a callback to be triggered when deferred deeplink is received.

#### Changed
- Removed MAC MD5 tracking feature for iOS platform completely.
- Updated docs.
- Updated native iOS SDK to version **4.7.1**.
- Updated native Android SDK to version **4.7.0**.

---

### Version 4.0.1 (29th October 2015)
#### Fixed
- Adjusted Adjust.framework for Windows users not to break symbolic links on Windows PC.

#### Changed
- Using native libraries release versions.
- Updated native iOS SDK to version **4.4.1**.
- Updated native Android SDK to version **4.1.3**.

---

### Version 4.0.0 (19th October 2015)
#### Added
- Added support for iOS 9.
- Added big code refactoring and new native SDK's 4.0.0 features (please check `migrate.md`).

#### Changed
- Updated native iOS SDK to version **4.4.0**.
- Updated native Android SDK to version **4.1.2**.

---

### Version 3.4.0 (7th October 2015)
#### Added
- Initial release of the adjust SDK for Marmalade.
- Added support for iOS and Android platforms.
- Added native iOS SDK version **3.4.0**.
- Added native Android SDK version **3.5.0**.
