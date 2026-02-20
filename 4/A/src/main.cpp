#include <climits>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>
#include <string>
#include <cstddef>
#include <cmath>
#include <numeric>

/**
 * @enum RespCode
 * @brief Возможные значения response'a для 1 позиции
 * @details Будем использовать такую форму, потому что работа с битовыми операциями быстрее
 * 
 * @var RespCode::RESP_MISS
 * @brief Такого символа нет в слове
 * 
 * @var RespCode::RESP_EXIST
 * @brief Такой символ есть в слове, но на другой позиции
 * 
 * @var RespCode::RESP_MATCH
 * @brief Попадание и символом и с позицией
 */
enum RespCodeAlpha {
    RESP_MISS = 0b00,   ///< -
    RESP_EXIST = 0b01,  ///< ?
    RESP_MATCH = 0b10,  ///< #
};

/// @brief Тип закодированного response'а, считаем, что длина слова не больше 8, тогда помещается в 
/// uint16_t. Для удобства будем считать что RespCodeAlpha занимает 2 бита информации, 
/// чтобы работать битовыми сдвигами
using RespCode = uint16_t;

/**
 * @brief Парсинг response'а из строки в наш числовой формат
 * 
 * @param resp - строка ответ из алфавита '-?#'
 * @param word_size - размер слова (длина строки resp)
 * 
 * @return RespCode
 */
RespCode ParseResp(std::string_view resp, size_t word_size);

/**
 * @brief Симулирует работу судьи. Даёт response по ответу и загадонному слову
 * 
 * @param resp - строка ответ из алфавита '-?#'
 * @param word_size - размер слова (длина строки resp)
 *
 * @details 
 * 1) Считаем частоту встречания каждого символа answer
 * 2) Смотрим точные совпаданения. Если нашли, то ставим # и уменьшаем частоту этой буквы на 1
 * 3) Смотрим неточные совпадения. Если нашли, что буква имеет ненулевую частоту, тогда ставим ? 
 * и уменьшаем частоту этой буквы на 1
 * 
 * @return RespCode
 */
RespCode GetResp(std::string_view answer, std::string_view guess, size_t word_size);

/**
 * @brief Выбор догадки через эверистику
 * 
 * @param dict - ссылка на список слов
 * @param candidate_inds - ссылка на список индексов слов в dict, которые могут являться загаднным 
 * словом
 * @param word_size - размер слова
 * 
 * @return Выбранная догадка
 * 
 * @details Если буква уникальная, то прибавляем к ней частоту встречания среди других слов 
 * на этом месте, потому что если буква популярная на этом месте, то это хорошо.
 * Если есть повторная буква, то делим, ну на 2, она имеет меньше веса.
 */
std::string SelectBestUniqueGuess(
    const std::vector<std::string>& dict, 
    const std::vector<size_t>& candidate_inds, 
    size_t word_size
);

/**
 * @brief Выбор догадки алгоритмом минимакс
 * 
 * @param dict - ссылка на список слов
 * @param candidate_inds - ссылка на список индексов слов в dict, которые могут являться загаднным 
 * словом
 * @param word_size - размер слова
 * 
 * @return Выбранная догадка
 * 
 * @details Каждое слово из словаря рассматриваем как возможная догадка. 
 * Считаем в массиве количество слов из возможных кандидатов на ответ для каждой маски. 
 * Таким образом получаем распределение "хорошести" догадки для каждой группы. 
 * Мы хотим минизировать худший случай, если попадём в группу с масимальным колчеиством вариантов, 
 * чтобы их было минимальное количество. 
 * Поэтому ищем минимум из максимумов размеров групп для каждой догадки и выбираем её.
 * 
 * @note Рассматривать весь словарь в качестве догадок слишком долго. 
 * Мы можем себе это позволить только если кандидатов достаточно мало, 
 * это задаётся константной LIM4USE_FULL_DICT, которая была подобрана мучитильными перезасылками.
 */
std::string SelectBestMinimaxGuess(
    const std::vector<std::string>& dict, 
    const std::vector<size_t>& candidate_inds, 
    size_t word_size
);

/**
 * @brief Выбор наилучшей догадки
 * 
 * @param dict - ссылка на список слов
 * @param candidate_inds - ссылка на список индексов слов в dict, которые могут являться загаднным 
 * словом
 * @param word_size - размер слова
 * 
 * @return Выбранная догадка
 * 
 * @details Если канидатов <= 2, то берём просто первого кандидата
 * Если кандидатов достаточно мало, а именно <= LIM4USE_MINIMAX, тогда используеем алгоритм минимакс
 * Если кандидатов много, то используем эвристический подход
 */
std::string SelectBestGuess(
    const std::vector<std::string>& dict, 
    const std::vector<size_t>& candidate_inds, 
    size_t word_size
);

/**
 * @brief Убирает из candidate_inds неподходящих под маску после ответа судьи кандидатов
 * 
 * @param dict - ссылка на список слов
 * @param candidate_inds - ссылка на список индексов слов в dict, которые могут являться загаднным 
 * словом
 * @param word_size - размер слова
 * @param guess - догадка, которую заслали
 * @param resp - ответ судьи, который мы получили на догадку guess
 * 
 * @details Заведём 2 индекса, по одному из них будем читать (read_ind), 
 * по другому - писать (write_ind).
 * read_ind двигаем всегда на единичку и пробегаем всех кандитов, 
 * чтобы проверить попали ли они в группу resp, полученную от судьи на заданный guess.
 * Когда нашли слово из нужной группы, записываем его в тот же массив candidate_inds по write_ind
 * и увеличиваем write_ind на 1. 
 * В конце ресайзим candidate_inds на write_ind, то есть на количество слов, которое у нас осталось
 *
 * @note write_ind <= read_ind всегда, потому что write_ind мы инкрементируем не всегда, 
 * а read_ind - всегда. Случай, когда они равны нам не страшен, потому что мы сначала читаем, 
 * потом обрабатываем то что, прочитали, а только потом записываем.
 */
void FilterCandidates(
    const std::vector<std::string>& dict, 
    std::vector<size_t>& candidate_inds, 
    size_t word_size,
    std::string_view guess,
    RespCode resp
);


// =================================================================================================
// Реализация функций
// =================================================================================================

RespCode ParseResp(std::string_view resp, size_t word_size) {
    RespCode code = 0;
    
    for (size_t ch_ind = 0; ch_ind < word_size; ++ch_ind) {
        RespCode val = RESP_MISS;
        if      (resp[ch_ind] == '#') val = RESP_MATCH;
        else if (resp[ch_ind] == '?') val = RESP_EXIST;
        code |= static_cast<RespCode>(val << (2 * ch_ind));
    }
    return code;
}

RespCode GetResp(std::string_view answer, std::string_view guess, size_t word_size) {

    int letter_count[std::numeric_limits<unsigned char>::max() + 1] = {0};

    for (size_t i = 0; i < word_size; ++i) {
        ++letter_count[static_cast<unsigned char>(answer[i])];
    }

    RespCode code = 0;

    // Точные совпадения
    for (size_t ch_ind = 0; ch_ind < word_size; ++ch_ind) {
        if (guess[ch_ind] == answer[ch_ind]) {
            code |= static_cast<RespCode>(RESP_MATCH << (2 * ch_ind));
            --letter_count[static_cast<unsigned char>(guess[ch_ind])];
        }
    }
    
    // Буквы на других позициях
    for (size_t ch_ind = 0; ch_ind < word_size; ++ch_ind) {
        if (guess[ch_ind] != answer[ch_ind]) {
            unsigned char ch = static_cast<unsigned char>(guess[ch_ind]);
            if (letter_count[ch] > 0) {
                code |= static_cast<RespCode>(RESP_EXIST << (2 * ch_ind));
                --letter_count[ch];
            }
        }
    }
    return code;
}

std::string SelectBestUniqueGuess(
    const std::vector<std::string>& dict, 
    const std::vector<size_t>& candidate_inds, 
    size_t word_size
) {
    std::vector<std::vector<long long>> freq(
        std::numeric_limits<unsigned char>::max() + 1, 
        std::vector<long long>(word_size, 0)
    );
    
    for (size_t candidate_ind : candidate_inds) {
        const std::string& candidate = dict[candidate_ind];
        for (size_t ch_ind = 0; ch_ind < word_size; ++ch_ind) {
            freq[(unsigned char)candidate[ch_ind]][ch_ind]++;
        }
    }

    std::string best_word = dict[candidate_inds[0]];
    long long best_score = -1;

    for (size_t candidate_ind : candidate_inds) {
        const std::string& word = dict[candidate_ind];
        long long cur_score = 0;
        bool seen[std::numeric_limits<unsigned char>::max() + 1] = {false};
        
        for (size_t ch_ind = 0; ch_ind < word_size; ++ch_ind) {
            unsigned char c = static_cast<unsigned char>(word[ch_ind]);
            if (!seen[c]) {
                cur_score += freq[c][ch_ind]; 
                seen[c] = true;
            } else {
                cur_score += freq[c][ch_ind] / 2;
            }
        }

        if (cur_score > best_score) {
            best_score = cur_score;
            best_word = word;
        }
    }
    return best_word;
}

std::string SelectBestMinimaxGuess(
    const std::vector<std::string>& dict, 
    const std::vector<size_t>& candidate_inds, 
    size_t word_size
) {
    const size_t candidates_cnt = candidate_inds.size();
    
    size_t best_max_group_size = std::numeric_limits<size_t>::max();
    size_t best_guess_ind = candidate_inds[0];
    
    constexpr size_t LIM4USE_FULL_DICT = 5;
    const bool is_search_full_dict = (candidates_cnt <= LIM4USE_FULL_DICT);
    const size_t guesses_cnt = is_search_full_dict ? dict.size() : candidates_cnt;
    const size_t groups_cnt = 1ull << (2 * word_size);

    std::vector<size_t> groups(groups_cnt, 0);
    
    for (size_t ind = 0; ind < guesses_cnt; ++ind) {
        const size_t guess_ind = is_search_full_dict ? ind : candidate_inds[ind];
        const std::string& guess_word = dict[guess_ind];
        
        std::fill(groups.begin(), groups.end(), 0);

        size_t cur_max = 0; 

        for (size_t candidate_ind : candidate_inds) {
            const std::string& candidate = dict[candidate_ind];
            
            RespCode code = GetResp(candidate, guess_word, word_size);
            groups[code]++;

            // Если уже больше лучшего из рассмотренных случаев, дальше считать смысла нет
            if (groups[code] > best_max_group_size) [[unlikely]] {
                cur_max = best_max_group_size;
                break;
            }

            // Обновляем максимум
            if (groups[code] > cur_max) {
                cur_max = groups[code];
            }
        }

        // Обновляем минимум максимумов
        if (cur_max < best_max_group_size) {
            best_max_group_size = cur_max;
            best_guess_ind = guess_ind;
        }
    }

    return dict[best_guess_ind];
}

std::string SelectBestGuess(
    const std::vector<std::string>& dict, 
    const std::vector<size_t>& candidate_inds, 
    size_t word_size
) {
    constexpr size_t LIM4USE_MINIMAX = 200;
    
    if (candidate_inds.size() <= 2) {
        return dict[candidate_inds[0]];
    } 
    else if (candidate_inds.size() > LIM4USE_MINIMAX) {
        return SelectBestUniqueGuess(dict, candidate_inds, word_size);
    }

    return SelectBestMinimaxGuess(dict, candidate_inds, word_size);
    
}

void FilterCandidates(
    const std::vector<std::string>& dict, 
    std::vector<size_t>& candidate_inds, 
    size_t word_size,
    std::string_view guess,
    RespCode resp
) {
    size_t write_ind = 0;
    for (size_t read_ind = 0; read_ind < candidate_inds.size(); ++read_ind) {
        size_t word_ind = candidate_inds[read_ind];
        if (GetResp(dict[word_ind], guess, word_size) == resp) {
            candidate_inds[write_ind++] = word_ind;
        }
    }
    
    candidate_inds.resize(write_ind);
}

// =================================================================================================
// main
// =================================================================================================

int main() {    
    constexpr size_t MAX_POSSIBLE_ANSWERS = 2050;
    constexpr size_t MAX_ATTEMPT_CNT = 6;

    size_t dict_size, rounds_cnt, word_size;
    std::cin >> dict_size >> rounds_cnt >> word_size;
    
    std::vector<std::string> dict;
    dict.reserve(dict_size);
    
    for (size_t i = 0; i < dict_size; ++i) {
        std::string word;
        std::cin >> word;
        dict.push_back(std::move(word));
    }
    
    for (size_t round = 0; round < rounds_cnt; ++round) {
        const size_t candidates_cnt = std::min(dict_size, MAX_POSSIBLE_ANSWERS);
        std::vector<size_t> candidate_inds(candidates_cnt);

        // Заполняет как 0, 1, 2, ..., вдруг они это написали мудрее чем цикл
        std::iota(candidate_inds.begin(), candidate_inds.end(), 0);
        
        for (size_t attempt = 0; attempt < MAX_ATTEMPT_CNT; ++attempt) {

            std::string cur_guess = SelectBestGuess(dict, candidate_inds, word_size);
            
            std::cout << cur_guess << std::endl;
            
            std::string resp;
            std::cin >> resp;
            
            // Если угадали
            if (resp.find_first_not_of('#') == std::string::npos) break; 

            const RespCode parsed_resp = ParseResp(resp, word_size);
            FilterCandidates(dict, candidate_inds, word_size, cur_guess, parsed_resp);
        }
    }
    
    return 0;
}