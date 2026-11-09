use calculator_rust_winui3::{CalculatorEngine, CalculatorMode};

fn main() {
    let mut engine = CalculatorEngine::new();
    engine.set_mode(CalculatorMode::Standard);

    println!("Standard mode demo");
    for input in ["2 + 2", "3 * 4", "10 / 2", "(2 + 3) * 4"] {
        match engine.evaluate_expression(input) {
            Ok(value) => println!("{input} = {value}"),
            Err(err) => println!("{input} -> error: {err}"),
        }
    }

    engine.memory_add(25.0);
    engine.memory_add(5.5);
    println!("memory = {}", engine.memory());
}
