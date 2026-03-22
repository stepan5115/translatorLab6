#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}🔨 Сборка проекта...${NC}"
echo -e "${CYAN}========================================${NC}"

make clean
if make all; then
    echo -e "${GREEN}✅ Сборка успешно завершена${NC}\n"
else
    echo -e "${RED}❌ Ошибка сборки проекта${NC}"
    exit 1
fi

if [ ! -f "./lab6" ]; then
    echo -e "${RED}❌ Исполняемый файл lab6 не найден${NC}"
    exit 1
fi

run_tests() {
    local test_dir=$1
    local expected_result=$2
    local test_type=$3
    
    local color_header=""
    local color_result=""
    
    if [ "$test_type" = "VALID" ]; then
        color_header="${GREEN}"
        color_result="${GREEN}"
    else
        color_header="${RED}"
        color_result="${RED}"
    fi
    
    echo -e "${color_header}========================================${NC}"
    echo -e "${color_header}📋 Запуск ${test_type} тестов${NC}"
    echo -e "${color_header}========================================${NC}"
    
    local total=0
    local passed=0
    local failed=0
    
    for test_file in "$test_dir"/*; do
        if [ -f "$test_file" ]; then
            total=$((total + 1))
            test_name=$(basename "$test_file")
            
            echo -ne "${YELLOW}Тест ${total}:${NC} ${test_name} ... "
            
            output=$(./lab6 "$test_file" 2>&1)
            exit_code=$?
            
            if [ "$expected_result" = "success" ]; then
                if [ $exit_code -eq 0 ]; then
                    echo -e "${GREEN}✓ УСПЕХ (программа завершилась без ошибок)${NC}"
                    passed=$((passed + 1))
                else
                    echo -e "${RED}✗ НЕУДАЧА (ожидался успех, но программа завершилась с кодом $exit_code)${NC}"
                    echo -e "${BLUE}Вывод программы:${NC}"
                    echo "$output" | sed 's/^/  /'
                    failed=$((failed + 1))
                fi
            else
                if [ $exit_code -ne 0 ]; then
                    echo -e "${GREEN}✓ УСПЕХ (программа правильно обнаружила ошибку, код $exit_code)${NC}"
                    passed=$((passed + 1))
                else
                    echo -e "${RED}✗ НЕУДАЧА (ожидалась ошибка, но программа завершилась успешно)${NC}"
                    failed=$((failed + 1))
                fi
            fi
        fi
    done
    
    echo -e "\n${CYAN}📊 Статистика для ${test_type} тестов:${NC}"
    echo -e "  Всего: ${total}"
    echo -e "  ${GREEN}Пройдено: ${passed}${NC}"
    echo -e "  ${RED}Провалено: ${failed}${NC}"
    
    if [ $failed -eq 0 ]; then
        echo -e "${GREEN}✨ Все ${test_type} тесты пройдены!${NC}\n"
    else
        echo -e "${RED}⚠️  Есть проваленные ${test_type} тесты${NC}\n"
    fi
    
    return $failed
}

run_tests "tests/valid" "success" "VALID"
valid_failed=$?

run_tests "tests/invalid" "error" "INVALID"
invalid_failed=$?

echo -e "${PURPLE}========================================${NC}"
echo -e "${PURPLE}📈 ИТОГОВЫЙ ОТЧЕТ${NC}"
echo -e "${PURPLE}========================================${NC}"

if [ $valid_failed -eq 0 ] && [ $invalid_failed -eq 0 ]; then
    echo -e "${GREEN}🎉 ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ! 🎉${NC}"
    exit 0
else
    echo -e "${RED}❌ НЕКОТОРЫЕ ТЕСТЫ ПРОВАЛЕНЫ ❌${NC}"
    exit 1
fi