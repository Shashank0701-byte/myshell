#!/bin/bash
# Test script for signal handling in myshell

echo "========================================="
echo "Signal Handling Test for myshell"
echo "========================================="
echo ""
echo "This script demonstrates that the shell"
echo "properly handles Ctrl+C (SIGINT)"
echo ""
echo "Instructions:"
echo "1. The shell will start"
echo "2. Run: sleep 5"
echo "3. Press Ctrl+C during sleep"
echo "4. Shell should survive and show prompt"
echo "5. Type 'exit' to quit"
echo ""
echo "Starting myshell..."
echo "========================================="
echo ""

./myshell
