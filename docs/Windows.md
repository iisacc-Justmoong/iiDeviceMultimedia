# Windows device/media build

Windows 하드웨어 백엔드는 Windows SDK의 C++/WinRT 헤더를 사용한다. Visual Studio 개발 환경에서 MSVC C++23으로 `cmake -S . -B build -G Ninja -DBUILD_TESTING=ON`을 실행한 뒤 빌드하고 CTest로 런타임과 하드웨어 계약을 검증한다. MinGW Qt 키트와 별도로 빌드한다. 속성 키 헤더를 명시적으로 포함하여 Windows SDK 헤더의 암묵적 포함 순서에 의존하지 않는다.
# USB 장치 인터페이스 GUID

Windows SDK의 `GUID_DEVINTERFACE_USB_DEVICE` 정의는 Microsoft가 지정한 순서대로 `initguid.h` 뒤에 `usbiodef.h`를 포함한다. USB 장치 열거를 사용하는 DLL과 별도 실행 파일을 함께 빌드하고 하드웨어 런타임 테스트를 실행하여 링크 및 실제 장치 열거를 검증한다.

장치 DLL은 Core와 독립적이며 표준 스레드와 Windows 네이티브 API만 사용한다. 일반 런타임·프레임 루프·표준 컨테이너 회귀 검사는 Core에서 별도로 수행한다. 장치 기능의 실제 사용 가능 여부는 대상 Windows의 권한·드라이버·서비스와 실물 장치에서 검증한다.

최신 구성·실행 결과와 복구 조치는 [Windows 빌드 및 실행](WindowsBuildAndRun.md)에 기록한다.
