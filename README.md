<h1 align="center">OpenDW</h1>
<p align="center">An open source reimplementation of Deepworld</p>
<p align="center">
  <a href="https://github.com/kuroppoi/opendw/actions"><img src="https://img.shields.io/github/actions/workflow/status/kuroppoi/opendw/build-win32.yml?labelColor=333A41&logoColor=959DA5&logo=github&style=plastic"/></a>
  <a href="https://github.com/kuroppoi/opendw/releases/latest"><img src="https://img.shields.io/github/v/release/kuroppoi/opendw?labelColor=333A41&logoColor=959DA5&logo=github&style=plastic"/></a>
  <a href="https://github.com/kuroppoi/opendw/tree/master/Source"><img src="https://img.shields.io/github/languages/code-size/kuroppoi/opendw?labelColor=333A41&style=plastic"/></a>
</p>
<p align="center">
    <kbd><img width="1536" height="864" alt="image" src="https://github.com/user-attachments/assets/309f7c82-48ac-4d02-942f-a2af50606efb"/></kbd>
</p>

> [!NOTE]
> Deepworld was originally developed for MacOS and iOS using an older version of Cocos2d.\
> Due to engine and platform differences, this project doesn't aim to be a 100% accurate recreation.

> [!IMPORTANT]
> Deepworld is an online game and requires a compatible [server](https://github.com/kuroppoi/brainwine) in order to be playable.

## Building

### Prerequisites
- Visual Studio 2022
- Axmol Engine [v2.8.1](https://github.com/axmolengine/axmol/releases/tag/v2.8.1)

> [!IMPORTANT]  
> Game assets are not included, so if you want to build from source you will have to provide them yourself.\
> That said, there are a few things to be aware of if you do so:
> - All audio files have to be converted to `.ogg`
> - Specifically, `title-fs8.png` has to be renamed to `title.png`
> - Skeleton data has to be updated to support Spine 4.2

### Instructions

<details>
<summary>Windows</summary>

```bash
git clone https://github.com/kuroppoi/opendw.git
cd opendw
axmol build -p win32
```
</details>

<details>
<summary>Other platforms</summary>

Good luck! :)
</details>
