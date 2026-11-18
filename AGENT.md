# iiDeviceMultimedia

This module owns USB/HID, BLE central/GATT, float32 PCM and MIDI UMP device I/O
and their protocol codecs. It and iiSDKCore are independent peer packages: no
production include, symbol, target, package or runtime dependency in either
direction. General runtime queues, tasks and Channel I/O remain in Core. Device
collection/dispatch is host-driven through Hardware::poll(); do not duplicate a
general scheduler or task runtime here.

Use native C++23, standard types and private platform bridges without Qt. Preserve
the device/protocol behavior. Public types and errors belong to `ii::device` and
`ii::device::protocol`; headers belong under `iiDeviceMultimedia/`. Public complete records are final and composition is
used instead of inheritance. The always-on architecture gate also preprocesses
Objective-C++ and applies when tests are disabled.

Preserve explicit bounded dispatch, cancellation, endpoint identity, callback
retirement and capture-before-capacity cleanup. Reject recursive/concurrent polls;
foreign active-poll hub close stops admission and returns busy for owner retry.
External driver cancellation/quiescence belongs to the application. Do not alter hardware behavior to
make extraction tests pass. Keep device capabilities separate from permissions,
presence and physical-device proof. Android/WASM hardware operations remain
explicitly unsupported until real host bridges exist.

Update documentation and tests with source changes; build only under build/.
Verify standalone installed consumption with Core package lookup disabled, the
Core-only boundary and the separate composition consumer in both package/include
orders. Integration belongs only in that consumer/application; no optional Core
adapter header or find_dependency is allowed in the device package. The private
Apple notification pump owns a joined standard thread, and native BLE callback
retirement must not depend on a Core TaskScope.
README.md is English. Do not disturb active apps, unrelated changes or hardware.

## 라이선스

이 저장소의 iiDeviceMultimedia는 iiSDK 모듈이며 Apache License 2.0이다. 자사 코드, 문서, 도구, 테스트가 포함된다. 저작권 표시는 `Copyright (c) 2026 iisacc`이고 고지는 루트 `NOTICE`에 있다. 사람용 한국어 설명은 [docs/License.md](docs/License.md)이다.

`SDK` 아래의 모든 iiSDK 모듈은 Apache-2.0이다. `Product` 아래의 iisacc 제품은 GNU Affero General Public License 버전 3 또는 그 이후 버전(AGPL-3.0-or-later)이다.

제3자와 벤더 코드는 원래 라이선스를 유지한다. 문서 재편 중에도 그 라이선스와 외부 원본은 보존한다.

<!-- workspace-docs:agent:start -->
## 문서 구조와 에이전트 탐색

- 프로젝트 루트의 `README.md`는 사람용 영문 진입 문서이며 `AGENT.md`는 에이전트 진입점이다.
- 하위 인간용 Markdown과 구조화된 agent JSON을 함께 유지한다. Markdown이 원본이며 JSON은 원문 해시와 줄 범위를 가진 파생 문서이다.
- 먼저 [docs/agent-index.json](docs/agent-index.json)에서 관련 문서와 JSON 투영을 찾는다. 원문 SHA-256이 다르면 현재 Markdown을 읽고 투영을 갱신한다.
- JSON의 규칙 후보는 검색용 어휘 일치 결과이다. 과거 기록·예제·실패 상태를 현재 지시나 검증 완료로 자동 해석하지 않는다.
- 기존 프로젝트별 규칙, 동결된 증거, 라이선스, 외부 원본, 사용자 데이터와 다른 작업의 변경을 보존한다.
- 이 문서 재편 지침은 과거 작업의 기능 구현·배포를 자동 재개하라는 요청이 아니다. 현재 사용자의 작업 범위를 우선한다.
- 문서를 수정하면 JSON 쌍·색인을 함께 갱신하고 검증한다. 기존 소스 변경에 대한 빌드·테스트 규칙은 계속 따른다.

Workspace에서 이 프로젝트의 문서를 갱신하는 명령은 다음과 같다.

```sh
python3 ../../Engineering/WorkspaceDocumentation/App/workspace_docs.py --workspace ../.. sync --project SDK/iiDeviceMultimedia
```
<!-- workspace-docs:agent:end -->
