use std::borrow::Cow;

fn process_data(data: Cow<str>) {
    let processed_data = data.to_uppercase();
    println!("Processed data: {}", processed_data);
}

fn main() {
    let borrowed_data: Cow<str> = Cow::Borrowed("Jai Shree Ram!");
    process_data(borrowed_data);

    let owned_data: Cow<str> = Cow::Owned(String::from("Jai Bajarang Bali!"));
    process_data(owned_data);
}
/*
amit@DESKTOP-9LTOFUP:~/OmPracticeRust/SP/Cow$ ./Cow
Processed data: JAI SHREE RAM!
Processed data: JAI BAJARANG BALI!
*/
