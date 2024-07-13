def calculate_average(numbers):
    total = 0
    for number in numbers:
        total += number
    # 小bug: 错误地使用了整数除法，这在Python 3中会导致精度丢失
    average = total // len(numbers)
    return average

# 示例
numbers = [10, 20, 30, 40, 50]
print(f"Average: {calculate_average(numbers)}")
