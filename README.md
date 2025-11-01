<h1 align="center">OpenDW</h1>
<p align="center">An open source reimplementation of Deepworld</p>

> [!NOTE]
> Deepworld was originally developed for MacOS and iOS using an older version of Cocos2d.\
> Due to engine and platform differences, this project doesn't aim to be a 100% accurate recreation.

> [!IMPORTANT]
> Deepworld is an online game and requires a [server](https://github.com/kuroppoi/brainwine) in order to be playable.

## Building

### Prerequisites
- Visual Studio 2022
- Axmol Engine [v2.8.1](https://github.com/axmolengine/axmol/releases/tag/v2.8.1)

> [!IMPORTANT]  
> Game assets are not included, so if you want to build from source you will have to provide them yourself.\
> That said, there are a few things to be aware of if you do so:
> - All audio files have to be converted to `.ogg`
> - Specifically, `title-fs8.png` has to be renamed to `title.png`
> - Local skeleton files from an original client will not work, so please use the ones in the [game config](https://github.com/kuroppoi/deepworld-config/blob/master/config-spine.yml).

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
