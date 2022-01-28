#pragma once

#include <memory>

#include "object.h"
#include <tokenizer.h>

std::shared_ptr<Object> Read(TokenParser* tokenizer);

std::shared_ptr<Object> ReadList(TokenParser* tokenizer);

void ReadLast(TokenParser* tokenizer, std::shared_ptr<Cell>& tail);