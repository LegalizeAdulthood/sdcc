# File to log all main make output to
BOOTSTRAPLOG=$(TOPDIR)/build.log
# Machine to ssh into to send the build result out via email
BOOTSTRAPSSHMAILSERVER=shell1.sourceforge.net
# Address to send the filtered build output to
BOOTSTRAPFILTEREDLIST=sdcc-devel@lists.sourceforge.net
# Address to send the unfiltered build output to
#BOOTSTRAPLIST=sdcc-buildlog@lists.sourceforge.net
BOOTSTRAPLIST=michaelh@juju.net.nz
# Subject line to use in the build output email
BOOTSTRAPSUBJECT=Automated build output ($(TARGET))
# Stamp to append to the build name.
BUILDDATE=$(shell date +%Y%m%d)
# Name of the tarball for this target
TARBALLNAME=$(TOPDIR)/build/sdcc-snapshot-$(TARGET)-$(BUILDDATE).tar.gz
# Location to copy the tarball to
SNAPSHOTDEST=shell1.sourceforge.net:/home/groups/s/sd/sdcc/htdocs/snapshots/$(TARGET)

update-bootstrap:
	cp -f sdcc-build-bootstrap.sh ..

# PENDING: Better naming
crontab-spawn: update-bootstrap logged-build generate-tarball upload-tarball send-build-mail kill-ssh-agent

logged-build:
	-$(MAKE) build > $(BOOTSTRAPLOG) 2>&1

generate-tarball:
	cd $(BUILDDIR); cd ..; tar czf $(TARBALLNAME) sdcc

upload-tarball:
	scp $(TARBALLNAME) $(SNAPSHOTDEST)

send-build-mail:
	cat $(BOOTSTRAPLOG) | ssh $(BOOTSTRAPSSHMAILSERVER) 'mail -s "$(BOOTSTRAPSUBJECT)" $(BOOTSTRAPLIST)'
#	cat $(BOOTSTRAPLOG) | egrep -v -f $(TOPDIR)/support/error-filter.sh | ssh $(BOOTSTRAPSSHMAILSERVER) 'mail -s "$(BOOTSTRAPSUBJECT)" $(BOOTSTRAPLIST)'

kill-ssh-agent:
	ssh-agent -k

