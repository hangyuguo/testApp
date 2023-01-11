use std::io;//prelude
use rand::Rng;//trait
use std::cmp::Ordering;

fn main() {
    println!("猜数字游戏!");
    println!("请猜测一个数字:");

    let foo = 1;//rust的变量默认都是不可变的 immutable
    //foo = 2;//会报错

    let secretNum = rand::thread_rng().gen_range(1, 101);
    println!("神秘数字是:{}",secretNum);


    let mut guess = String::new();// mut 标记可变变量

    io::stdin().read_line(&mut guess).expect("无法读取行");

    println!("你猜测的数是:{}",guess);

    // shadow 复用guess 从这一行以后 guess就变成新的类型了
    let guess:u32 = guess.trim().parse().expect("输入的不是整数");

    match guess.cmp(&secretNum){
        Ordering::Less => println!("Too small!"),
        Ordering::Greater => println!("Too big!"),
        Ordering::Equal => println!("You win!"),
    }
}
