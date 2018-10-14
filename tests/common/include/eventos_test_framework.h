/*
    EventOS V1.0 - Copyright (C) 2014 Edielson Prevato Frigieri.

       This file is part of the EventOS distribution.

    EventTOS is free software; you can redistribute it and/or modify it under
    the terms of the MIT License (MIT).
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

 */

/**
 * @file eventos_test_framework.h
 * @brief Header file for test framework.
 */

#ifndef _EVENTOS_TEST_FRAMEWORK_H_
#define _EVENTOS_TEST_FRAMEWORK_H_

void TEST_CacheResult( char cResult );

void TEST_SubmitResultBuffer();

void TEST_NotifyTestStart();

void TEST_NotifyTestFinished();

void TEST_SubmitResult( const char * pcResult );

#endif /* _EVENTOS_TEST_FRAMEWORK_H_ */
