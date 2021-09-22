# Toy Renderer

孙嘉锴，3180105871

## 目录

[TOC]

## 实验要求

使用OpenGL渲染一个Sketchfab上的模型，要求效果一致

## 实验原理

### OpenGL

#### 简介

引用Khronos官网对OpenGL的介绍

> OpenGL® is the most widely adopted 2D and 3D graphics API in the industry, bringing thousands of applications to a wide variety of computer platforms. It is window-system and operating-system independent as well as network-transparent. OpenGL enables developers of software for PC, workstation, and supercomputing hardware to create high-performance, visually compelling graphics software applications, in markets such as CAD, content creation, energy, entertainment, game development, manufacturing, medical, and virtual reality. OpenGL exposes all the features of the latest graphics hardware.

简单来说，OpenGL是一个被广泛应用的2D/3D图形API

为了使用采用核心模式（Core-profile mode）而非立即渲染模式（Immediate mode）的绘制模式，本次实验我使用的是OpenGL 3.3

#### 渲染管线

![image-20210922143324721](C:\Users\45098\AppData\Roaming\Typora\typora-user-images\image-20210922143324721.png)

图形渲染管线的第一个部分是顶点着色器，它把一个单独的顶点作为输入。顶点着色器主要的目的是把局部坐标转化为裁剪坐标，同时顶点着色器允许我们对顶点属性进行一些基本处理。

图元装配阶段将顶点着色器输出的所有顶点作为输入，并所有的点装配成指定图元的形状。

图元装配阶段的输出会传递给几何着色器。几何着色器把图元形式的一系列顶点的集合作为输入，它可以通过产生新顶点构造出新的（或是其它的）图元来生成其他形状。

几何着色器的输出会被传入光栅化阶段，这里它会把图元映射为最终屏幕上相应的像素，生成供片段着色器使用的片段。在片段着色器运行之前会执行裁切。裁切会丢弃超出视图以外的所有像素，用来提升执行效率。

片段着色器的主要目的是计算一个像素的最终颜色。

在所有对应颜色值确定以后，最终的对象将会被传到最后一个阶段，进行模板测试、深度测试、Alpha测试，最后再混合。

### GLFW

GLFW是一个专门针对OpenGL的C语言库，它提供了一些渲染物体所需的最低限度的接口。

本次实验我使用GLFW作为窗口管理、输入处理等的框架

### GLAD

引用Github仓库中README的介绍

> Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs.

简单来说，GLAD帮我们处理了GL函数装载、生成等繁琐的问题

### Assimp

引用官网的介绍

> The **Open Asset Import Library** (short name: Assimp) is a portable Open-Source library to import various well-known [3D model formats](https://github.com/assimp/assimp) in a uniform manner.

简单来说，是一个开源的3D模型导入，导入后数据模型如下

![image-20210922144623030](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922144623030.png)

本次实验我使用Assimp进行模型导入

### Dear ImGui

引用Github仓库中README的介绍

> Dear ImGui is a **bloat-free graphical user interface library for C++**. It outputs optimized vertex buffers that you can render anytime in your 3D-pipeline enabled application. It is fast, portable, renderer agnostic and self-contained (no external dependencies).
>
> Dear ImGui is designed to **enable fast iterations** and to **empower programmers** to create **content creation tools and visualization / debug tools** (as opposed to UI for the average end-user). It favors simplicity and productivity toward this goal, and lacks certain features normally found in more high-level libraries.
>
> Dear ImGui is particularly suited to integration in games engine (for tooling), real-time 3D applications, fullscreen applications, embedded applications, or any applications on consoles platforms where operating system features are non-standard.

简单来说，Dear ImGui是一个开箱即用的GUI库，可以很方便的用在我们的渲染器中来调整各种参数

### Gamma校正

#### Gamma值

Gamma也叫灰度系数，每种显示设备都有自己的Gamma值，都不相同，有一个公式：设备输出亮度 = 电压的Gamma次幂，任何设备Gamma基本上都不会等于1，等于1是一种理想的线性状态，这种理想状态是：如果电压和亮度都是在0到1的区间，那么多少电压就等于多少亮度。

#### 人眼感知

![image-20210922154519874](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922154519874.png)

由于人眼对颜色/灰阶的感知并不与物理亮度线性相关，而对比较暗的颜色变化更敏感。

#### 校正

所以为了让我们在线性空间计算的颜色值被人眼更“正确”的感知到，所以我们需要在计算结束后进行Gamma校正

![image-20210922154733359](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922154733359.png)

#### sRGB纹理

艺术家们制作的纹理贴图是在sRGB空间的（因为艺术家们是根据屏幕效果来进行调整的），所以我们在读取时要进行逆向Gamma校正

### 阴影

#### 阴影映射

我们可以利用显卡的深度缓冲来进行阴影映射：在真正的渲染之前，先在光的位置为视角进行渲染，记录每个片段的深度值作为阴影贴图。然后在真正的渲染时，将世界坐标转化为以光为视角的坐标，在阴影纹理中采样，若其采样值小于其深度值，说明它被遮挡，光不在此片段着色

![image-20210922160358345](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922160358345.png)

#### 阴影失真

在实现上述阴影映射后，基本都会出现阴影失真的情况（我的项目中也出现了，但我在后续修复了，所以截图使用教程中的截图）

![image-20210922160506864](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922160506864.png)

这是因为阴影贴图分辨率不够，出现了如下的情况

![image-20210922160533733](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922160533733.png)

在距离光源比较远的情况下，多个片段可能从深度贴图的同一个值中去采样，图片每个斜坡代表深度贴图一个单独的纹理像素，可以看到，多个片段从同一个深度值进行采样，这就导致在同一深度值进行采样的临近片段，有些可能被视为处于阴影中，而有些没有。

我们可以采用一种非常Tricky的方法——阴影偏移，来解决上述问题

![image-20210922160728211](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922160728211.png)

我们对阴影贴图应用一个很小的偏移量即可

但是有些表面坡度很大，仍然会产生阴影失真，需要更大的偏移量，所以我们采用下述方法来动态修改偏移量

```c++
float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
```

### 基于物理的着色

**注**：在实验初期我采用的是Blinn-Phong模型，后期逐渐改写为PBR着色

一个基于物理的着色模型，需要满足以下三个条件：

1. 基于微平面(Microfacet)的表面模型。
2. 能量守恒。
3. 应用基于物理的BRDF。

#### 微表面模型

所有的PBR技术都基于微平面理论。这项理论认为，达到微观尺度之后任何平面都可以用被称为微平面的细小镜面来进行描绘。根据平面粗糙程度的不同，这些细小镜面的取向排列可以相当不一致：一个平面越是粗糙，这个平面上的微平面的排列就越混乱。这些微小镜面这样无序取向排列的影响就是，当我们特指镜面光/镜面反射时，入射光线更趋向于向完全不同的方向发散开来，进而产生出分布范围更广泛的反射。而与之相反的是，对于一个光滑的平面，光线大体上会更趋向于向同一个方向反射，造成更小更锐利的反射

![image-20210922145515511](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922145515511.png)

我们采用统计学的方法来概略的估算微平面的粗糙程度：我们可以基于一个平面的粗糙度来计算出半程向量与微平面平均取向方向一致的概率，下图是不同粗糙度对着色影响的例子：

![image-20210922145942163](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922145942163.png)

#### 能量守恒

为了遵守能量守恒定律，我们需要对漫反射光和镜面反射光之间做出明确的区分。当一束光线碰撞到一个表面的时候，它就会分离成一个折射部分和一个反射部分。反射部分就是会直接反射开来而不会进入平面的那部分光线，这就是我们所说的镜面光照。而折射部分就是余下的会进入表面并被吸收的那部分光线，这也就是我们所说的漫反射光照。

![image-20210922145952854](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922145952854.png)

我们按照能量守恒的关系，首先计算镜面反射部分，它的值等于入射光线被反射的能量所占的百分比。然后折射光部分就可以直接由镜面反射部分计算得出：

```c++
float kS = 计算镜面部分...
float kD = 1.0 - kS
```

#### 渲染方程

对一个没有自发光的物体，渲染方程为

![image-20210922150249277](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922150249277.png)

如果有自发光，在等式右侧的积分之前加入自发光项即可

上式中，$$L_o$$与$$Li$$代表某点$$p$$某方向（立体角）上的的辐射率（Radiance），$$\omega_i$$与$$\omega_o$$代表入射与出射方向（立体角），$$n$$为点$$p$$的法向量，$$f_r$$为BRDF(Bidirectional Reflective Distribution Function，双向反射分布函数)函数

#### BRDF

本次实验中，我使用的是Cook-Torrance模型，因此只介绍Cook-Torrance模型

![image-20210922150932510](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922150932510.png)

这里的$k_d$是早先提到过的入射光线中被折射部分的能量所占的比率，而$k_s$是被反射部分的比率。

BRDF的左侧表示的是漫反射部分，这里用$f_{lambert}$来表示。它被称为Lambertian漫反射，用如下的公式来表示：

![image-20210922151035681](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922151035681.png)

BRDF的右侧表示的是镜面反射部分，这里用$f_{cook-torrance}$来表示，其公式为：

![image-20210922151150836](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922151150836.png)

$D$代表法向量分布函数(Normal **D**istribution Function)，$F$代表菲涅尔方程(**F**resnel Rquation)，$G$代表几何函数(**G**eometry Function)

* **法向量分布函数**：估算微平面平均取向方向与半程向量一致的概率，受表面粗糙度参数影响
* **菲涅尔方程**：菲涅尔方程描述的是在不同的表面角下表面所反射的光线所占的比率
* **几何函数**：当一个平面相对比较粗糙的时候，平面表面上的微平面有可能挡住其他的微平面从而减少表面所反射的光线，而几何函数就是来描述这个的

以上三种函数都有各自的形式，在本次实验中，我采用Trowbridge-Reitz GGX作为法向量分布函数，Fresnel-Schlick近似作为菲涅尔方程，Smith’s Schlick-GGX作为几何函数

##### 法向量分布函数

![image-20210922151727806](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922151727806.png)

此处$n$为法向量（微平面平均取向方向）、$h$为半程向量，$\alpha$为粗糙度参数

##### 菲涅尔方程

菲涅尔（发音为Freh-nel）方程描述的是被反射的光线对比光线被折射的部分所占的比率，这个比率会随着我们观察的角度不同而不同。利用这个反射比率和能量守恒原则，我们可以直接得出光线被折射的部分以及光线剩余的能量。

菲涅尔方程非常复杂：

![image-20210922152359486](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922152359486.png)

在上式中，$\theta_i$为入射光与法线的夹角，$\theta_t$为折射光与法线的夹角，$n_1$为入射光所在介质折射率，$n_2$为入射物体折射率，考虑到光的偏振，$R_s$与$R_p$分别代表S偏振光和P偏振光在入射角为$\theta_i$时反射光所占比率。

由于上述计算太过复杂，我们可以使用Fresnel-Schlick来近似计算菲涅尔方程

![image-20210922152845963](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922152845963.png)

但是Fresnel-Schlick近似只对非导体表面有意义（计算结果接近），为了可以计算导体表面，我们传入表面对于法向入射$F_0$的反应，然后利用Fresnel-Schlick近似进行插值计算即可

##### 几何函数

为了更好的理解几何函数，先展示一张图

![image-20210922153402841](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922153402841.png)

可以看到，微表面有可能相互遮蔽，而几何函数就是计算相互遮蔽的概率的。

![image-20210922153503110](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922153503110.png)

这里的$k$是$\alpha$基于几何函数是针对直接光照还是针对IBL光照的重映射 :

![image-20210922153608842](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922153608842.png)

由于观察方向和光线方向都有可能产生遮蔽现象。所以我们要把两者都考虑进去

![image-20210922153736582](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922153736582.png)

#### PBR材质

![image-20210922153803821](D:\Programs\LearnOpenGL\Toy Renderer.assets\image-20210922153803821.png)

为了有效利用PBR着色的各种参数，我们一般使用五种纹理贴图

* **反照率**(Albedo)：反照率(Albedo)纹理为每一个金属的纹素(Texel)（纹理像素）指定表面颜色或者基础反射率。

* **法线**(Normal)：法线贴图使我们可以逐片段的指定独特的法线，来为表面制造出起伏不平的假象。

* **金属度**(Metallic)：金属贴图逐个纹素的指定该纹素是不是金属质地的。根据PBR引擎设置的不同，美术师们既可以将金属度编写为灰度值又可以编写为1或0这样的二元值。

* **粗糙度**(Roughness)：粗糙度贴图可以以纹素为单位指定某个表面有多粗糙。采样得来的粗糙度数值会影响一个表面的微平面统计学上的取向度。一个比较粗糙的表面会得到更宽阔更模糊的镜面反射（高光），而一个比较光滑的表面则会得到集中而清晰的镜面反射。

* **AO**(Ambient Occlusion)：环境光遮蔽贴图或者说AO贴图为表面和周围潜在的几何图形指定了一个额外的阴影因子。比如如果我们有一个砖块表面，反照率纹理上的砖块裂缝部分应该没有任何阴影信息。然而AO贴图则会把那些光线较难逃逸出来的暗色边缘指定出来。



