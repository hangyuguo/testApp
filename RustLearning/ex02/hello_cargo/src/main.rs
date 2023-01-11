fn main() {
    println!("Hello, world!!");
}

//Cargo.toml 是cargo配置文件 在项目顶层目录下
//main.rs 位于src目录下

//crate 代码的包 （如第三方库）

//cargo new project_name  初始化项目
//cargo build  创建可执行程序 生成在debug目录
//cargo build --release 生成在release目录 并且编译过程中会优化，时间也更长
//cargo run   构建并运行可执行程序 
//cargo check 检查代码 但不生成可执行文件 速度很快 常用于开发时检查