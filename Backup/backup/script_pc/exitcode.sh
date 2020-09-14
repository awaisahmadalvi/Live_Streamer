#!/bin/bash
trap 'echo 42; exit 42' SIGINT SIGQUIT SIGTERM
until(1);do
{
	sleep 1
}
done
