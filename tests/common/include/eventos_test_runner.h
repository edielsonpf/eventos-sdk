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
 * @file eventos_test_runner.h
 * @brief The function to be called to run all the tests.
 */

#ifndef _EVENTOS_TEST_RUNNER_H_
#define _EVENTOS_TEST_RUNNER_H_

#include "eventos_test_runner_config.h"

/**
 * @brief Size of shared array.
 *
 */
#define testrunnerBUFFER_SIZE    ( 4000 )

/**
 * @brief Buffer used for all tests.
 *
 * Since tests are run in series, they can use the same memory array.
 * This makes significant heap savings.
 */
extern char cBuffer[ testrunnerBUFFER_SIZE ];

/**
 * @brief EventOS heap measurement taken before tests are run.
 */
extern unsigned int xHeapBefore;

/**
 * @brief EventOS heap measurement taken after all tests are run.
 */
extern unsigned int xHeapAfter;


/**
 * @brief Runs all the tests.
 */
void TEST_RUNNER_RunTests( void );



#endif /* _EVENTOS_TEST_RUNNER_H_ */
