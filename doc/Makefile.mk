.PHONY: doc

doc-clean:
	rm -rf doc/{html,man,latex} $(PACKAGE_DOCNAME).tar*

doc: all
	@cd doc && doxygen && cp -fr data html/

clean-local: doc-clean

