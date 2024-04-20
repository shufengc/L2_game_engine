TARGET = game_engine_linux

default: release

debug:
	@$(MAKE) -C src debug
	@cp src/$(TARGET) $(TARGET)

gprof:
	@$(MAKE) -C src gprof
	@cp src/$(TARGET) $(TARGET)
	./$(TARGET)
	gprof $(TARGET) gmon.out | tee out.txt | gprof2dot | dot -Tsvg -o output.svg

release:
	@$(MAKE) -C src release
	@cp src/$(TARGET) $(TARGET)

clean:
	@$(MAKE) -C src clean
	@rm -f $(TARGET)


.PHONY: default debug release clean
